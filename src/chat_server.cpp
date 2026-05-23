#include "chat_server.hpp"
#include "utils.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace { constexpr int BUFFER_SIZE = 2048; }

ChatServer::ChatServer(int port, const std::string& log_file)
    : port_(port), server_fd_(-1), running_(false), logger_(log_file) {}

ChatServer::~ChatServer() { stop(); }

void ChatServer::start() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) throw std::runtime_error("Failed to create server socket");

    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_);

    if (bind(server_fd_, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0)
        throw std::runtime_error("Bind failed");
    if (listen(server_fd_, 10) < 0)
        throw std::runtime_error("Listen failed");

    running_ = true;
    logger_.info("Server started on port " + std::to_string(port_));
    accept_loop();
}

void ChatServer::stop() {
    if (!running_) return;
    running_ = false;
    if (server_fd_ >= 0) close(server_fd_);
    {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        for (auto& [fd, client] : clients_) close(fd);
        clients_.clear();
    }
    for (auto& worker : workers_) if (worker.joinable()) worker.join();
    logger_.info("Server stopped");
}

void ChatServer::accept_loop() {
    while (running_) {
        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        int client_fd = accept(server_fd_, reinterpret_cast<sockaddr*>(&client_addr), &len);
        if (client_fd < 0) continue;
        std::string ip = inet_ntoa(client_addr.sin_addr);
        logger_.info("Incoming connection from " + ip);
        workers_.emplace_back(&ChatServer::handle_client, this, client_fd, ip);
    }
}

void ChatServer::handle_client(int client_fd, std::string client_addr) {
    char buffer[BUFFER_SIZE];
    std::string username = "guest-" + std::to_string(client_fd);

    int bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes <= 0) { close(client_fd); return; }
    buffer[bytes] = '\0';
    std::string intro = trim(buffer);
    if (intro.rfind("USER ", 0) == 0) username = trim(intro.substr(5));

    {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        clients_[client_fd] = {client_fd, username, client_addr};
    }

    send_to_client(client_fd, "[server] Welcome " + username + "\n");
    broadcast("[server] " + username + " joined from " + client_addr + "\n", client_fd);
    logger_.info(username + " joined from " + client_addr);

    while (running_) {
        std::memset(buffer, 0, sizeof(buffer));
        bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';
        std::string msg = trim(buffer);
        if (msg.empty()) continue;
        if (msg == "/quit") break;
        if (msg == "/list") {
            std::string users = "[server] Active users: ";
            std::lock_guard<std::mutex> lock(clients_mutex_);
            bool first = true;
            for (const auto& [fd, info] : clients_) {
                if (!first) users += ", ";
                users += info.username;
                first = false;
            }
            users += "\n";
            send_to_client(client_fd, users);
            continue;
        }
        std::string formatted = "[" + username + "] " + msg + "\n";
        broadcast(formatted, client_fd);
        send_to_client(client_fd, formatted);
        logger_.info("message from " + username + ": " + msg);
    }

    broadcast("[server] " + username + " left the chat\n", client_fd);
    logger_.warn(username + " disconnected");
    remove_client(client_fd);
    close(client_fd);
}

void ChatServer::broadcast(const std::string& message, int exclude_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (const auto& [fd, client] : clients_) {
        if (fd == exclude_fd) continue;
        send(fd, message.c_str(), message.size(), 0);
    }
}

void ChatServer::send_to_client(int client_fd, const std::string& message) {
    send(client_fd, message.c_str(), message.size(), 0);
}

void ChatServer::remove_client(int client_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    clients_.erase(client_fd);
}
