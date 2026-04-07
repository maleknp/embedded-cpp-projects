#pragma once

#include "drivers/sensor_base.hpp"
#include <random>

namespace etc {

class PressureSensor : public ISensor {
public:
    bool init() override;
    float read() override;
    std::string getName() const override { return "PressureSensor"; }

private:
    bool initialized_ = false;
    float base_pressure_ = 1013.25f;   // hPa
    std::mt19937 rng_{std::random_device{}()};
    std::normal_distribution<float> noise_{0.0f, 1.5f};
};

} // namespace etc
