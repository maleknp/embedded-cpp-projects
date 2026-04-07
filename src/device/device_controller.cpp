#include "device/device_controller.hpp"
#include "utils/logger.hpp"
#include <cstring>
#include <thread>
#include <chrono>

namespace etc {

DeviceController::DeviceController()
    : eeprom_("eeprom.dat") {}

bool DeviceController::init() {
    Logger::instance().info("DEV", "======================================");
    Logger::instance().info("DEV", "  Embedded Telemetry Controller v1.0  ");
    Logger::instance().info("DEV", "======================================");

    // Load or create EEPROM config
    if (!eeprom_.load()) {
        eeprom_.loadDefaults();
        eeprom_.save();
    }

    device_id_ = 0x01;
    Logger::instance().info("DEV", "Device ID: 0x01");
    Logger::instance().info("DEV", "Device name: " + eeprom_.read("device_name", "UNKNOWN"));

    // Initialize sensors
    bool ok = true;
    ok &= temp_sensor_.init();
    ok &= press_sensor_.init();
    ok &= batt_monitor_.init();

    if (!ok) {
        Logger::instance().error("DEV", "Sensor initialization failed!");
        sm_.transition(DeviceEvent::FAULT);
        return false;
    }

    // Boot complete → IDLE
    sm_.transition(DeviceEvent::BOOT_COMPLETE);

    // Setup periodic tasks
    setupScheduler();

    Logger::instance().info("DEV", "Initialization complete. Device is IDLE.");
    return true;
}

void DeviceController::setupScheduler() {
    uint32_t sample_rate = std::stoul(eeprom_.read("sample_rate", "1000"));

    scheduler_.addTask("ReadSensors", [this]() { taskReadSensors(); }, sample_rate);
    scheduler_.addTask("SendTelemetry", [this]() { taskSendTelemetry(); }, sample_rate * 2);
    scheduler_.addTask("ProcessCommands", [this]() { taskProcessCommands(); }, 100);
}

void DeviceController::taskReadSensors() {
    if (sm_.getState() != DeviceState::ACTIVE) return;

    temp_sensor_.read();
    press_sensor_.read();
    batt_monitor_.read();
}

void DeviceController::taskSendTelemetry() {
    if (sm_.getState() != DeviceState::ACTIVE) return;
    if (eeprom_.read("telemetry_enabled", "1") != "1") return;

    Packet pkt = collectTelemetry();
    auto frame = pkt.encode();
    uart_.send(frame);
}

void DeviceController::taskProcessCommands() {
    if (uart_.rxPending() == 0) return;

    auto frame = uart_.receive();
    auto pkt_opt = Packet::decode(frame);

    if (!pkt_opt) {
        Logger::instance().warn("DEV", "Received invalid packet — discarding");
        return;
    }

    Logger::instance().info("DEV", "Received command: " + Packet::commandName(pkt_opt->command));
    Packet response = processCommand(*pkt_opt);
    auto resp_frame = response.encode();
    uart_.send(resp_frame);
}

Packet DeviceController::processCommand(const Packet& cmd) {
    uint8_t base_cmd = cmd.command & 0x7F;

    switch (base_cmd) {
        case static_cast<uint8_t>(Command::GET_STATUS):
            return buildStatusResponse();

        case static_cast<uint8_t>(Command::GET_SENSOR_DATA):
            return buildSensorDataResponse();

        case static_cast<uint8_t>(Command::SET_CONFIG):
            return buildConfigResponse(cmd);

        case static_cast<uint8_t>(Command::RESET_DEVICE):
            return buildResetResponse();

        default:
            Logger::instance().warn("DEV", "Unknown command: " + Packet::commandName(cmd.command));
            Packet err;
            err.device_id = device_id_;
            err.command = cmd.command | static_cast<uint8_t>(Command::RESPONSE_FLAG);
            err.payload = {0xFF}; // Error code
            return err;
    }
}

Packet DeviceController::collectTelemetry() {
    float temp  = temp_sensor_.read();
    float press = press_sensor_.read();
    float batt  = batt_monitor_.read();

    Packet pkt;
    pkt.device_id = device_id_;
    pkt.command = static_cast<uint8_t>(Command::GET_SENSOR_DATA)
                | static_cast<uint8_t>(Command::RESPONSE_FLAG);

    // Pack floats as raw bytes: [temp(4)][press(4)][batt(4)][state(1)]
    pkt.payload.resize(13);
    std::memcpy(&pkt.payload[0], &temp, 4);
    std::memcpy(&pkt.payload[4], &press, 4);
    std::memcpy(&pkt.payload[8], &batt, 4);
    pkt.payload[12] = static_cast<uint8_t>(sm_.getState());

    return pkt;
}

Packet DeviceController::buildStatusResponse() const {
    Packet rsp;
    rsp.device_id = device_id_;
    rsp.command = static_cast<uint8_t>(Command::GET_STATUS)
                | static_cast<uint8_t>(Command::RESPONSE_FLAG);
    rsp.payload = {
        static_cast<uint8_t>(sm_.getState()),
        static_cast<uint8_t>(tick_count_ & 0xFF),
    };
    return rsp;
}

Packet DeviceController::buildSensorDataResponse() {
    return collectTelemetry();
}

Packet DeviceController::buildConfigResponse(const Packet& cmd) {
    // Payload format: [key_len][key_bytes][val_len][val_bytes]
    Packet rsp;
    rsp.device_id = device_id_;
    rsp.command = static_cast<uint8_t>(Command::SET_CONFIG)
                | static_cast<uint8_t>(Command::RESPONSE_FLAG);

    if (cmd.payload.size() < 2) {
        rsp.payload = {0xFF}; // Error
        return rsp;
    }

    uint8_t key_len = cmd.payload[0];
    if (cmd.payload.size() < static_cast<size_t>(1 + key_len + 1)) {
        rsp.payload = {0xFF};
        return rsp;
    }

    std::string key(cmd.payload.begin() + 1, cmd.payload.begin() + 1 + key_len);
    uint8_t val_len = cmd.payload[1 + key_len];
    std::string val(cmd.payload.begin() + 2 + key_len,
                    cmd.payload.begin() + 2 + key_len + val_len);

    eeprom_.write(key, val);
    eeprom_.save();

    Logger::instance().info("DEV", "Config updated: " + key + " = " + val);
    rsp.payload = {0x00}; // Success
    return rsp;
}

Packet DeviceController::buildResetResponse() {
    Logger::instance().info("DEV", "RESET command received — resetting device state");

    sm_.transition(DeviceEvent::STOP);   // ACTIVE → IDLE (if active)
    tick_count_ = 0;

    Packet rsp;
    rsp.device_id = device_id_;
    rsp.command = static_cast<uint8_t>(Command::RESET_DEVICE)
                | static_cast<uint8_t>(Command::RESPONSE_FLAG);
    rsp.payload = {0x00}; // Success
    return rsp;
}

void DeviceController::tick() {
    ++tick_count_;
    scheduler_.tick();
}

void DeviceController::runDemo(int iterations) {
    Logger::instance().info("DEV", "--- Starting demo mode (" + std::to_string(iterations) + " iterations) ---");

    // Move to ACTIVE state
    sm_.transition(DeviceEvent::START);

    // Simulate a host sending GET_STATUS command
    {
        Packet cmd;
        cmd.device_id = device_id_;
        cmd.command = static_cast<uint8_t>(Command::GET_STATUS);
        uart_.pushToRx(cmd.encode());
    }

    for (int i = 0; i < iterations; ++i) {
        tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // Inject a GET_SENSOR_DATA command halfway through
        if (i == iterations / 2) {
            Logger::instance().info("DEV", "--- Host sends GET_SENSOR_DATA ---");
            Packet cmd;
            cmd.device_id = device_id_;
            cmd.command = static_cast<uint8_t>(Command::GET_SENSOR_DATA);
            uart_.pushToRx(cmd.encode());
        }
    }

    // Move back to IDLE
    sm_.transition(DeviceEvent::STOP);
    Logger::instance().info("DEV", "--- Demo complete ---");

    // Print TX buffer summary
    Logger::instance().info("DEV", "TX buffer has " + std::to_string(uart_.txPending()) + " frames pending");
}

} // namespace etc
