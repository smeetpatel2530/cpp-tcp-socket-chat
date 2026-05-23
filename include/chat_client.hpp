#pragma once
#include "logger.hpp"
#include "platform.hpp"
#include <atomic>
#include <string>
#include <thread>

class ChatClient {
public:
    ChatClient(const std::string& host, int port,
               const std::string& username,
               const std::string& log_file);
    ~ChatClient();
    void run();
private:
    void receive_loop();
    void send_line(const std::string& line);

    std::string  host_;
    int          port_;
    std::string  username_;
    socket_t     socket_fd_;
    std::atomic<bool> running_;
    Logger       logger_;
    std::thread  receiver_;
};
