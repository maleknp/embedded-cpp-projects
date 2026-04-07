#include "comm/crc16.hpp"

namespace etc {

uint16_t CRC16::compute(const uint8_t* data, size_t length) {
    uint16_t crc = INIT;

    for (size_t i = 0; i < length; ++i) {
        crc ^= static_cast<uint16_t>(data[i]) << 8;
        for (int bit = 0; bit < 8; ++bit) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ POLY;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

uint16_t CRC16::compute(const std::vector<uint8_t>& data) {
    return compute(data.data(), data.size());
}

} // namespace etc
