#pragma once

#include "device/state_machine.hpp"
#include "device/scheduler.hpp"
#include "drivers/temperature_sensor.hpp"
#include "drivers/pressure_sensor.hpp"
#include "drivers/battery_monitor.hpp"
#include "comm/uart_sim.hpp"
#include "comm/packet.hpp"
#include "storage/eeprom_sim.hpp"

#include <memory>

namespace etc {

/// Central device controller — owns all subsystems, processes commands,
/// generates telemetry, and drives the main application loop.
class DeviceController {
public:
    DeviceController();

    /// Initialize all subsystems. Transitions from BOOT → IDLE.
    bool init();

    /// Process a single incoming command packet and return response packet.
    Packet processCommand(const Packet& cmd);

    /// Collect all sensor data and encode into a telemetry packet.
    Packet collectTelemetry();

    /// Run one iteration (tick) of the device. Called by the scheduler.
    void tick();

    /// Run the device for a specified number of iterations (demo mode).
    void runDemo(int iterations);

    /// Access subsystems (for testing / advanced use).
    StateMachine& stateMachine() { return sm_; }
    UARTSim& uart() { return uart_; }
    EEPROMSim& eeprom() { return eeprom_; }

private:
    StateMachine sm_;
    Scheduler scheduler_;
    UARTSim uart_;
    EEPROMSim eeprom_;

    TemperatureSensor temp_sensor_;
    PressureSensor press_sensor_;
    BatteryMonitor batt_monitor_;

    uint8_t device_id_ = 0x01;
    int tick_count_ = 0;

    void setupScheduler();
    void taskReadSensors();
    void taskSendTelemetry();
    void taskProcessCommands();

    Packet buildStatusResponse() const;
    Packet buildSensorDataResponse();
    Packet buildConfigResponse(const Packet& cmd);
    Packet buildResetResponse();
};

} // namespace etc
