#include "storage/eeprom_sim.hpp"
#include "utils/logger.hpp"
#include <fstream>
#include <sstream>

namespace etc {

EEPROMSim::EEPROMSim(const std::string& filepath)
    : filepath_(filepath) {}

bool EEPROMSim::load() {
    std::ifstream file(filepath_);
    if (!file.is_open()) {
        Logger::instance().warn("EEPROM", "File not found: " + filepath_ + " — using defaults");
        return false;
    }

    data_.clear();
    std::string line;
    while (std::getline(file, line)) {
        auto pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string val = line.substr(pos + 1);
            data_[key] = val;
        }
    }

    Logger::instance().info("EEPROM", "Loaded " + std::to_string(data_.size()) + " entries from " + filepath_);
    return true;
}

bool EEPROMSim::save() {
    std::ofstream file(filepath_);
    if (!file.is_open()) {
        Logger::instance().error("EEPROM", "Failed to write: " + filepath_);
        return false;
    }

    for (const auto& [key, val] : data_) {
        file << key << "=" << val << "\n";
    }

    Logger::instance().info("EEPROM", "Saved " + std::to_string(data_.size()) + " entries to " + filepath_);
    return true;
}

std::string EEPROMSim::read(const std::string& key, const std::string& default_val) const {
    auto it = data_.find(key);
    return (it != data_.end()) ? it->second : default_val;
}

void EEPROMSim::write(const std::string& key, const std::string& value) {
    data_[key] = value;
    Logger::instance().debug("EEPROM", "Write: " + key + " = " + value);
}

bool EEPROMSim::hasKey(const std::string& key) const {
    return data_.find(key) != data_.end();
}

void EEPROMSim::loadDefaults() {
    if (!hasKey("sample_rate"))       write("sample_rate", "1000");
    if (!hasKey("device_name"))       write("device_name", "ETC-001");
    if (!hasKey("telemetry_enabled")) write("telemetry_enabled", "1");
    Logger::instance().info("EEPROM", "Defaults loaded");
}

} // namespace etc
