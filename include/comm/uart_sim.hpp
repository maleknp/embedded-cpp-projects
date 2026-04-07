#pragma once

#include <cstdint>
#include <vector>
#include <queue>
#include <mutex>
#include <optional>

namespace etc {

/// Simulates a UART peripheral with TX and RX FIFOs.
class UARTSim {
public:
    /// Enable loopback mode: TX bytes are automatically pushed to RX.
    void setLoopback(bool enable) { loopback_ = enable; }

    /// Transmit a frame (push bytes to TX FIFO).
    void send(const std::vector<uint8_t>& data);

    /// Receive a complete frame from the RX FIFO.
    /// Returns empty vector if nothing available.
    std::vector<uint8_t> receive();

    /// Push a frame into RX from the external side (host → device).
    void pushToRx(const std::vector<uint8_t>& data);

    /// Check how many frames are waiting in the TX FIFO.
    size_t txPending() const;

    /// Check how many frames are waiting in the RX FIFO.
    size_t rxPending() const;

    /// Pop the next frame from TX (host reads device output).
    std::vector<uint8_t> popFromTx();

private:
    std::queue<std::vector<uint8_t>> tx_fifo_;
    std::queue<std::vector<uint8_t>> rx_fifo_;
    bool loopback_ = false;
    mutable std::mutex mutex_;
};

} // namespace etc
