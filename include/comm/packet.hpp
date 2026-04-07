#pragma once

#include <cstdint>
#include <vector>
#include <optional>
#include <string>

namespace etc {

/// Command IDs used in the protocol.
enum class Command : uint8_t {
    GET_STATUS      = 0x01,
    GET_SENSOR_DATA = 0x02,
    SET_CONFIG      = 0x03,
    RESET_DEVICE    = 0x04,
    // Responses use the command | 0x80
    RESPONSE_FLAG   = 0x80,
};

/// Represents a decoded UART packet.
struct Packet {
    uint8_t device_id = 0x01;
    uint8_t command   = 0x00;
    std::vector<uint8_t> payload;

    /// Header bytes
    static constexpr uint8_t HEADER_0 = 0xAA;
    static constexpr uint8_t HEADER_1 = 0x55;

    /// Encode this packet into a raw byte vector:
    /// [HEADER_0][HEADER_1][LEN][DEV_ID][CMD][PAYLOAD...][CRC_HI][CRC_LO]
    /// LEN = number of bytes from DEV_ID to end of PAYLOAD (inclusive).
    std::vector<uint8_t> encode() const;

    /// Try to decode a raw byte vector into a Packet.
    /// Returns std::nullopt on failure (bad header, CRC mismatch, too short).
    static std::optional<Packet> decode(const std::vector<uint8_t>& raw);

    /// Get human-readable command name.
    static std::string commandName(uint8_t cmd);
};

} // namespace etc
