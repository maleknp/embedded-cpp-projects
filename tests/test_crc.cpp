#include <gtest/gtest.h>
#include "comm/crc16.hpp"
#include <vector>

using namespace etc;

// ── Known Test Vectors ─────────────────────────────────────────────────────────

TEST(CRC16Test, EmptyData) {
    std::vector<uint8_t> data {};
    uint16_t crc = CRC16::compute(data);
    // CRC-16/CCITT-FALSE of empty data = 0xFFFF (init value, no processing)
    EXPECT_EQ(crc, 0xFFFF);
}

TEST(CRC16Test, StandardTestVector) {
    // "123456789" → CRC-16/CCITT-FALSE = 0x29B1
    std::vector<uint8_t> data = {'1','2','3','4','5','6','7','8','9'};
    uint16_t crc = CRC16::compute(data);
    EXPECT_EQ(crc, 0x29B1);
}

TEST(CRC16Test, SingleByte) {
    std::vector<uint8_t> data = {0x00};
    uint16_t crc = CRC16::compute(data);
    // Known value for single zero byte
    EXPECT_NE(crc, 0xFFFF); // Must differ from init
    EXPECT_EQ(crc, 0xE1F0);
}

TEST(CRC16Test, AllOnes) {
    std::vector<uint8_t> data = {0xFF, 0xFF, 0xFF, 0xFF};
    uint16_t crc = CRC16::compute(data);
    // Must be deterministic
    uint16_t crc2 = CRC16::compute(data);
    EXPECT_EQ(crc, crc2);
}

TEST(CRC16Test, PointerInterface) {
    std::vector<uint8_t> data = {'1','2','3','4','5','6','7','8','9'};
    uint16_t crc_vec = CRC16::compute(data);
    uint16_t crc_ptr = CRC16::compute(data.data(), data.size());
    EXPECT_EQ(crc_vec, crc_ptr);
}

TEST(CRC16Test, DifferentDataDifferentCRC) {
    std::vector<uint8_t> a = {0x01, 0x02, 0x03};
    std::vector<uint8_t> b = {0x01, 0x02, 0x04};
    EXPECT_NE(CRC16::compute(a), CRC16::compute(b));
}
