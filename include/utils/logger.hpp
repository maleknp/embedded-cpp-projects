#pragma once

#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace etc {

enum class LogLevel { DEBUG, INFO, WARN, ERROR };

class Logger {
public:
    static Logger& instance();

    void setLevel(LogLevel level);
    void log(LogLevel level, const std::string& module, const std::string& message);

    void debug(const std::string& module, const std::string& msg);
    void info(const std::string& module, const std::string& msg);
    void warn(const std::string& module, const std::string& msg);
    void error(const std::string& module, const std::string& msg);

private:
    Logger() = default;
    LogLevel min_level_ = LogLevel::DEBUG;

    static const char* levelToString(LogLevel level);
    static std::string timestamp();
};

} // namespace etc
