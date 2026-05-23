#include "chat_server.hpp"
#include "utils.hpp"
#include <cstring>
#include <stdexcept>

namespace { constexpr int BUFFER_SIZE = 2048; }

ChatServer::ChatServer(int port, const std::string& log_file)
    : port_(port), server_fd_(INVALID_SOCK), running_(false), logger_(log_file) {}

ChatServer::~ChatServer() { stop(); }

void ChatServer::start() {
    init_sockets();

    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ == INVALID_SOCK)
        throw std::runtime_error("Failed to create server socket: " + std::to_string(sock_error()));

    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR,
               reinterpret_cast<const char*>(&opt), sizeof(opt));

    sockaddr_in server_addr{};
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(static_cast<u_short>(port_));

    if (bind(server_fd_, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) != 0)
        throw std::runtime_error("Bind failed: " + std::to_string(sock_error()));
    if (listen(server_fd_, 10) != 0)
        throw std::runtime_error("Listen failed: " + std::to_string(sock_error()));

    running_ = true;
    logger_.info("Server listening on port " + std::to_string(port_));
    logger_.info("Waiting for clients...");
    accept_loop();
}

void ChatServer::stop() {
    if (!running_) return;
    running_ = false;
    if (server_fd_ != INVALID_SOCK) close_sock(server_fd_);
    {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        for (auto& [fd, client] : clients_) close_sock(fd);
        clients_.clear();
    }
    for (auto& w : workers_) if (w.joinable()) w.join();
    cleanup_sockets();
    logger_.info("Server stopped");
}

void ChatServer::accept_loop() {
    while (running_) {
        sockaddr_in client_addr{};
        socklen_t   len = sizeof(client_addr);
        socket_t client_fd = accept(server_fd_,
                                    reinterpret_cast<sockaddr*>(&client_addr), &len);
        if (client_fd == INVALID_SOCK) continue;

        char ip[INET_ADDRSTRLEN]{};
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
        std::string client_ip(ip);
        logger_.info("New connection from " + client_ip);

        workers_.emplace_back(&ChatServer::handle_client, this, client_fd, client_ip);
    }
}

void ChatServer::handle_client(socket_t client_fd, std::string client_addr) {
    char buffer[BUFFER_SIZE]{};
    std::string username = "guest-" + std::to_string((int)client_fd);

    // First message must be "USER <name>"
    int bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes <= 0) { close_sock(client_fd); return; }
    buffer[bytes] = '\0';
    std::string intro = trim(buffer);
    if (intro.rfind("USER ", 0) == 0) username = trim(intro.substr(5));

    {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        clients_[client_fd] = {client_fd, username, client_addr};
    }

    send_to_client(client_fd, "[server] Welcome, " + username + "!\n");
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
            std::string users = "[server] Online users: ";
            std::lock_guard<std::mutex> lock(clients_mutex_);
            bool first = true;
            for (auto& [fd, info] : clients_) {
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
        logger_.info(username + ": " + msg);
    }

    broadcast("[server] " + username + " left the chat.\n", client_fd);
    logger_.warn(username + " disconnected");
    remove_client(client_fd);
    close_sock(client_fd);
}

void ChatServer::broadcast(const std::string& message, socket_t exclude_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (auto& [fd, client] : clients_) {
        if (fd == exclude_fd) continue;
        send(fd, message.c_str(), static_cast<int>(message.size()), 0);
    }
}

void ChatServer::send_to_client(socket_t client_fd, const std::string& message) {
    send(client_fd, message.c_str(), static_cast<int>(message.size()), 0);
}

void ChatServer::remove_client(socket_t client_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    clients_.erase(client_fd);
}
