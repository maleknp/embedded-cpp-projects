#pragma once

#include "drivers/sensor_base.hpp"
#include <random>

namespace etc {

class BatteryMonitor : public ISensor {
public:
    bool init() override;
    float read() override;
    std::string getName() const override { return "BatteryMonitor"; }

private:
    bool initialized_ = false;
    float voltage_ = 4.2f;             // Start fully charged
    std::mt19937 rng_{std::random_device{}()};
    std::normal_distribution<float> noise_{0.0f, 0.02f};
    int read_count_ = 0;
};

} // namespace etc
