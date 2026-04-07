#include "drivers/battery_monitor.hpp"
#include "utils/logger.hpp"
#include <algorithm>

namespace etc {

bool BatteryMonitor::init() {
    Logger::instance().info("BATT", "Initializing battery monitor");
    initialized_ = true;
    return true;
}

float BatteryMonitor::read() {
    if (!initialized_) {
        Logger::instance().error("BATT", "Sensor not initialized!");
        return -999.0f;
    }

    // Simulate slow battery drain
    ++read_count_;
    if (read_count_ % 10 == 0) {
        voltage_ -= 0.01f;
    }

    float value = voltage_ + noise_(rng_);
    value = std::clamp(value, 3.0f, 4.2f);

    Logger::instance().debug("BATT", "Read: " + std::to_string(value) + " V");
    return value;
}

} // namespace etc
