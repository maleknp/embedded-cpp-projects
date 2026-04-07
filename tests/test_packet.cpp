#include <gtest/gtest.h>
#include "comm/packet.hpp"
#include <vector>
#include <cstdint>

using namespace etc;

// ── Encode / Decode Roundtrip ──────────────────────────────────────────────────

TEST(PacketTest, EncodeDecodeRoundtrip_NoPayload) {
    Packet pkt;
    pkt.device_id = 0x01;
    pkt.command = static_cast<uint8_t>(Command::GET_STATUS);

    auto encoded = pkt.encode();
    auto decoded = Packet::decode(encoded);

    ASSERT_TRUE(decoded.has_value());
    EXPECT_EQ(decoded->device_id, 0x01);
    EXPECT_EQ(decoded->command, static_cast<uint8_t>(Command::GET_STATUS));
    EXPECT_TRUE(decoded->payload.empty());
}

TEST(PacketTest, EncodeDecodeRoundtrip_WithPayload) {
    Packet pkt;
    pkt.device_id = 0x42;
    pkt.command = static_cast<uint8_t>(Command::SET_CONFIG);
    pkt.payload = {0x10, 0x20, 0x30, 0x40, 0x50};

    auto encoded = pkt.encode();
    auto decoded = Packet::decode(encoded);

    ASSERT_TRUE(decoded.has_value());
    EXPECT_EQ(decoded->device_id, 0x42);
    EXPECT_EQ(decoded->command, static_cast<uint8_t>(Command::SET_CONFIG));
    ASSERT_EQ(decoded->payload.size(), 5u);
    EXPECT_EQ(decoded->payload[0], 0x10);
    EXPECT_EQ(decoded->payload[4], 0x50);
}

// ── Error Cases ────────────────────────────────────────────────────────────────

TEST(PacketTest, DecodeFailsOnTooShortFrame) {
    std::vector<uint8_t> data = {0xAA, 0x55, 0x02};
    auto result = Packet::decode(data);
    EXPECT_FALSE(result.has_value());
}

TEST(PacketTest, DecodeFailsOnInvalidHeader) {
    std::vector<uint8_t> data = {0xBB, 0x55, 0x02, 0x01, 0x01, 0x00, 0x00};
    auto result = Packet::decode(data);
    EXPECT_FALSE(result.has_value());
}

TEST(PacketTest, DecodeFailsOnCRCMismatch) {
    Packet pkt;
    pkt.device_id = 0x01;
    pkt.command = static_cast<uint8_t>(Command::GET_STATUS);

    auto encoded = pkt.encode();
    // Corrupt the CRC
    encoded.back() ^= 0xFF;

    auto result = Packet::decode(encoded);
    EXPECT_FALSE(result.has_value());
}

TEST(PacketTest, DecodeFailsOnDataCorruption) {
    Packet pkt;
    pkt.device_id = 0x01;
    pkt.command = static_cast<uint8_t>(Command::GET_STATUS);
    pkt.payload = {0xDE, 0xAD};

    auto encoded = pkt.encode();
    // Corrupt a payload byte
    encoded[5] ^= 0x01;

    auto result = Packet::decode(encoded);
    EXPECT_FALSE(result.has_value());
}

// ── Header Validation ──────────────────────────────────────────────────────────

TEST(PacketTest, EncodedFrameHasCorrectHeader) {
    Packet pkt;
    pkt.device_id = 0x01;
    pkt.command = 0x01;

    auto encoded = pkt.encode();
    ASSERT_GE(encoded.size(), 2u);
    EXPECT_EQ(encoded[0], 0xAA);
    EXPECT_EQ(encoded[1], 0x55);
}

// ── Command Name Utility ───────────────────────────────────────────────────────

TEST(PacketTest, CommandNameReturnsCorrectStrings) {
    EXPECT_EQ(Packet::commandName(0x01), "GET_STATUS");
    EXPECT_EQ(Packet::commandName(0x02), "GET_SENSOR_DATA");
    EXPECT_EQ(Packet::commandName(0x81), "GET_STATUS_RSP");
    EXPECT_EQ(Packet::commandName(0x82), "GET_SENSOR_DATA_RSP");
}
