#pragma once
#include <fstream>
#include <mutex>
#include <string>

class Logger {
public:
    explicit Logger(const std::string& file_path);
    void info(const std::string& msg);
    void warn(const std::string& msg);
    void error(const std::string& msg);
private:
    void log(const std::string& level, const std::string& msg);
    std::ofstream file_;
    std::mutex    mutex_;
};
