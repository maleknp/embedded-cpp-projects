#include "comm/uart_sim.hpp"
#include "utils/logger.hpp"
#include <sstream>
#include <iomanip>

namespace etc {

void UARTSim::send(const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Log the hex bytes
    std::ostringstream oss;
    oss << "TX [" << data.size() << " bytes]:";
    for (auto b : data) {
        oss << " " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b);
    }
    Logger::instance().debug("UART", oss.str());

    tx_fifo_.push(data);

    if (loopback_) {
        rx_fifo_.push(data);
        Logger::instance().debug("UART", "Loopback: frame echoed to RX");
    }
}

std::vector<uint8_t> UARTSim::receive() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (rx_fifo_.empty()) return {};

    auto frame = rx_fifo_.front();
    rx_fifo_.pop();

    std::ostringstream oss;
    oss << "RX [" << frame.size() << " bytes]:";
    for (auto b : frame) {
        oss << " " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b);
    }
    Logger::instance().debug("UART", oss.str());

    return frame;
}

void UARTSim::pushToRx(const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    rx_fifo_.push(data);
}

size_t UARTSim::txPending() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return tx_fifo_.size();
}

size_t UARTSim::rxPending() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return rx_fifo_.size();
}

std::vector<uint8_t> UARTSim::popFromTx() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (tx_fifo_.empty()) return {};
    auto frame = tx_fifo_.front();
    tx_fifo_.pop();
    return frame;
}

} // namespace etc
