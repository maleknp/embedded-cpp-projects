#include "drivers/temperature_sensor.hpp"
#include "utils/logger.hpp"
#include <algorithm>

namespace etc {

bool TemperatureSensor::init() {
    Logger::instance().info("TEMP", "Initializing temperature sensor");
    initialized_ = true;
    return true;
}

float TemperatureSensor::read() {
    if (!initialized_) {
        Logger::instance().error("TEMP", "Sensor not initialized!");
        return -999.0f;
    }

    // Simulate slow drift + noise
    base_temp_ += noise_(rng_) * 0.01f;
    float value = base_temp_ + noise_(rng_);
    value = std::clamp(value, 20.0f, 45.0f);

    Logger::instance().debug("TEMP", "Read: " + std::to_string(value) + " °C");
    return value;
}

} // namespace etc
