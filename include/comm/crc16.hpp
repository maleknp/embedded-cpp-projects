#pragma once

#include <cstdint>
#include <vector>

namespace etc {

class CRC16 {
public:
    /// Compute CRC-16/CCITT-FALSE over the given data.
    /// Polynomial: 0x1021, Init: 0xFFFF
    static uint16_t compute(const uint8_t* data, size_t length);
    static uint16_t compute(const std::vector<uint8_t>& data);

private:
    static constexpr uint16_t POLY = 0x1021;
    static constexpr uint16_t INIT = 0xFFFF;
};

} // namespace etc
