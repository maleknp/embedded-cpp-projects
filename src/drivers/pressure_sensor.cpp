#include "drivers/pressure_sensor.hpp"
#include "utils/logger.hpp"
#include <algorithm>

namespace etc {

bool PressureSensor::init() {
    Logger::instance().info("PRESS", "Initializing pressure sensor");
    initialized_ = true;
    return true;
}

float PressureSensor::read() {
    if (!initialized_) {
        Logger::instance().error("PRESS", "Sensor not initialized!");
        return -999.0f;
    }

    base_pressure_ += noise_(rng_) * 0.005f;
    float value = base_pressure_ + noise_(rng_);
    value = std::clamp(value, 950.0f, 1050.0f);

    Logger::instance().debug("PRESS", "Read: " + std::to_string(value) + " hPa");
    return value;
}

} // namespace etc
