#include "chat_client.hpp"
#include "utils.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

namespace { constexpr int BUFFER_SIZE = 2048; }

ChatClient::ChatClient(const std::string& host, int port, const std::string& username, const std::string& log_file)
    : host_(host), port_(port), username_(username), socket_fd_(-1), running_(false), logger_(log_file) {}

ChatClient::~ChatClient() {
    running_ = false;
    if (socket_fd_ >= 0) close(socket_fd_);
    if (receiver_.joinable()) receiver_.join();
}

void ChatClient::run() {
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ < 0) throw std::runtime_error("Failed to create socket");

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    if (inet_pton(AF_INET, host_.c_str(), &server_addr.sin_addr) <= 0)
        throw std::runtime_error("Invalid server IP address");
    if (connect(socket_fd_, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0)
        throw std::runtime_error("Connection failed");

    running_ = true;
    send_line("USER " + username_ + "\n");
    logger_.info("Connected to server " + host_ + ":" + std::to_string(port_));

    receiver_ = std::thread(&ChatClient::receive_loop, this);

    std::cout << "Connected as " << username_ << ". Type messages, /list, or /quit\n";
    std::string line;
    while (running_ && std::getline(std::cin, line)) {
        line = trim(line);
        if (line.empty()) continue;
        send_line(line + "\n");
        if (line == "/quit") break;
    }
    running_ = false;
}

void ChatClient::receive_loop() {
    char buffer[BUFFER_SIZE];
    while (running_) {
        std::memset(buffer, 0, sizeof(buffer));
        int bytes = recv(socket_fd_, buffer, BUFFER_SIZE - 1, 0);
        if (bytes <= 0) {
            logger_.warn("Disconnected from server");
            running_ = false;
            break;
        }
        buffer[bytes] = '\0';
        std::cout << buffer;
    }
}

void ChatClient::send_line(const std::string& line) {
    send(socket_fd_, line.c_str(), line.size(), 0);
    logger_.info("sent: " + trim(line));
}
