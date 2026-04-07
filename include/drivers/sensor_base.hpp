#pragma once

#include <string>

namespace etc {

/// Abstract sensor interface — mimics an embedded HAL driver.
class ISensor {
public:
    virtual ~ISensor() = default;

    /// Initialize the sensor hardware (simulated).
    virtual bool init() = 0;

    /// Read the latest value from the sensor.
    virtual float read() = 0;

    /// Human-readable name for logging.
    virtual std::string getName() const = 0;
};

} // namespace etc
