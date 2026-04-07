#pragma once

#include <string>
#include <map>

namespace etc {

/// Simulates EEPROM/NVM storage as a file-backed key-value store.
class EEPROMSim {
public:
    explicit EEPROMSim(const std::string& filepath = "eeprom.dat");

    /// Load config from file. Returns false if file doesn't exist.
    bool load();

    /// Save current state to file. Returns false on I/O error.
    bool save();

    /// Read a config value. Returns default_val if key not found.
    std::string read(const std::string& key, const std::string& default_val = "") const;

    /// Write a config value (in-memory until save() is called).
    void write(const std::string& key, const std::string& value);

    /// Check if a key exists.
    bool hasKey(const std::string& key) const;

    /// Initialize with default configuration values.
    void loadDefaults();

private:
    std::string filepath_;
    std::map<std::string, std::string> data_;
};

} // namespace etc
