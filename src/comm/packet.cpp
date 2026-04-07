#include "comm/packet.hpp"
#include "comm/crc16.hpp"
#include "utils/logger.hpp"
#include <sstream>
#include <iomanip>

namespace etc {

std::vector<uint8_t> Packet::encode() const {
    // LEN = device_id(1) + command(1) + payload(N)
    uint8_t len = static_cast<uint8_t>(2 + payload.size());

    // Build the data over which CRC is computed: LEN + DEV_ID + CMD + PAYLOAD
    std::vector<uint8_t> crc_data;
    crc_data.reserve(1 + len);
    crc_data.push_back(len);
    crc_data.push_back(device_id);
    crc_data.push_back(command);
    crc_data.insert(crc_data.end(), payload.begin(), payload.end());

    uint16_t crc = CRC16::compute(crc_data);

    // Assemble full frame
    std::vector<uint8_t> frame;
    frame.reserve(2 + crc_data.size() + 2);
    frame.push_back(HEADER_0);
    frame.push_back(HEADER_1);
    frame.insert(frame.end(), crc_data.begin(), crc_data.end());
    frame.push_back(static_cast<uint8_t>(crc >> 8));   // CRC high
    frame.push_back(static_cast<uint8_t>(crc & 0xFF));  // CRC low

    return frame;
}

std::optional<Packet> Packet::decode(const std::vector<uint8_t>& raw) {
    // Minimum frame: HEADER(2) + LEN(1) + DEV(1) + CMD(1) + CRC(2) = 7
    if (raw.size() < 7) {
        Logger::instance().warn("PKT", "Frame too short: " + std::to_string(raw.size()) + " bytes");
        return std::nullopt;
    }

    // Validate header
    if (raw[0] != HEADER_0 || raw[1] != HEADER_1) {
        Logger::instance().warn("PKT", "Invalid header");
        return std::nullopt;
    }

    uint8_t len = raw[2];
    // Total expected size: 2(header) + 1(len) + len + 2(crc) = 5 + len
    if (raw.size() < static_cast<size_t>(5 + len)) {
        Logger::instance().warn("PKT", "Frame length mismatch");
        return std::nullopt;
    }

    // Extract CRC from frame
    size_t crc_offset = 3 + len;
    uint16_t received_crc = (static_cast<uint16_t>(raw[crc_offset]) << 8) |
                             static_cast<uint16_t>(raw[crc_offset + 1]);

    // Compute CRC over LEN + data
    std::vector<uint8_t> crc_data(raw.begin() + 2, raw.begin() + 3 + len);
    uint16_t computed_crc = CRC16::compute(crc_data);

    if (received_crc != computed_crc) {
        std::ostringstream oss;
        oss << "CRC mismatch: received 0x" << std::hex << std::setw(4) << std::setfill('0') << received_crc
            << " computed 0x" << std::setw(4) << std::setfill('0') << computed_crc;
        Logger::instance().warn("PKT", oss.str());
        return std::nullopt;
    }

    Packet pkt;
    pkt.device_id = raw[3];
    pkt.command   = raw[4];
    if (len > 2) {
        pkt.payload.assign(raw.begin() + 5, raw.begin() + 3 + len);
    }

    return pkt;
}

std::string Packet::commandName(uint8_t cmd) {
    uint8_t base = cmd & 0x7F;
    bool is_response = (cmd & 0x80) != 0;
    std::string name;

    switch (base) {
        case 0x01: name = "GET_STATUS";      break;
        case 0x02: name = "GET_SENSOR_DATA";  break;
        case 0x03: name = "SET_CONFIG";       break;
        case 0x04: name = "RESET_DEVICE";     break;
        default: {
            std::ostringstream oss;
            oss << "UNKNOWN(0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(cmd) << ")";
            return oss.str();
        }
    }

    return is_response ? name + "_RSP" : name;
}

} // namespace etc
