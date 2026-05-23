#pragma once
#include "logger.hpp"
#include "platform.hpp"
#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

struct ClientInfo {
    socket_t    socket_fd;
    std::string username;
    std::string address;
};

class ChatServer {
public:
    ChatServer(int port, const std::string& log_file);
    ~ChatServer();
    void start();
    void stop();
private:
    void accept_loop();
    void handle_client(socket_t client_fd, std::string client_addr);
    void broadcast(const std::string& message, socket_t exclude_fd = INVALID_SOCK);
    void send_to_client(socket_t client_fd, const std::string& message);
    void remove_client(socket_t client_fd);

    int              port_;
    socket_t         server_fd_;
    std::atomic<bool> running_;
    Logger           logger_;
    std::mutex       clients_mutex_;
    std::unordered_map<socket_t, ClientInfo> clients_;
    std::vector<std::thread> workers_;
};
