#include "utils.hpp"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string now_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto t   = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    tm = *std::localtime(&t);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string trim(const std::string& s) {
    auto start = std::find_if_not(s.begin(), s.end(),
                    [](unsigned char c){ return std::isspace(c); });
    auto end   = std::find_if_not(s.rbegin(), s.rend(),
                    [](unsigned char c){ return std::isspace(c); }).base();
    if (start >= end) return "";
    return std::string(start, end);
}

void init_sockets() {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
        throw std::runtime_error("WSAStartup failed");
#endif
}

void cleanup_sockets() {
#ifdef _WIN32
    WSACleanup();
#endif
}
