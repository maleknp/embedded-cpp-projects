#include "utils/logger.hpp"

namespace etc {

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::setLevel(LogLevel level) {
    min_level_ = level;
}

void Logger::log(LogLevel level, const std::string& module, const std::string& message) {
    if (level < min_level_) return;
    std::cout << timestamp()
              << " [" << levelToString(level) << "]"
              << " [" << module << "] "
              << message << "\n";
}

void Logger::debug(const std::string& module, const std::string& msg) { log(LogLevel::DEBUG, module, msg); }
void Logger::info(const std::string& module, const std::string& msg)  { log(LogLevel::INFO,  module, msg); }
void Logger::warn(const std::string& module, const std::string& msg)  { log(LogLevel::WARN,  module, msg); }
void Logger::error(const std::string& module, const std::string& msg) { log(LogLevel::ERROR, module, msg); }

const char* Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO ";
        case LogLevel::WARN:  return "WARN ";
        case LogLevel::ERROR: return "ERROR";
    }
    return "?????";
}

std::string Logger::timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t_now), "%H:%M:%S")
        << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

} // namespace etc
