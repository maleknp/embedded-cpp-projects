#pragma once

#include "drivers/sensor_base.hpp"
#include <random>

namespace etc {

class TemperatureSensor : public ISensor {
public:
    bool init() override;
    float read() override;
    std::string getName() const override { return "TemperatureSensor"; }

private:
    bool initialized_ = false;
    float base_temp_ = 25.0f;
    std::mt19937 rng_{std::random_device{}()};
    std::normal_distribution<float> noise_{0.0f, 0.5f};
};

} // namespace etc
