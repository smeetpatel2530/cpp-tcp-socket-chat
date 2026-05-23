#include "logger.hpp"
#include "utils.hpp"
#include <iostream>

Logger::Logger(const std::string& file_path)
    : file_(file_path, std::ios::app) {}

void Logger::info (const std::string& msg) { log("INFO ", msg); }
void Logger::warn (const std::string& msg) { log("WARN ", msg); }
void Logger::error(const std::string& msg) { log("ERROR", msg); }

void Logger::log(const std::string& level, const std::string& msg) {
    std::lock_guard<std::mutex> lock(mutex_);
    const std::string line = "[" + now_timestamp() + "] [" + level + "] " + msg;
    std::cout << line << "\n";
    if (file_.is_open()) file_ << line << "\n";
}
