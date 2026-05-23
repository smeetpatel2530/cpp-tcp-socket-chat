#include "chat_server.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    int port = 9090;
    if (argc > 1) port = std::stoi(argv[1]);
    try {
        ChatServer server(port, "server.log");
        server.start();
    } catch (const std::exception& ex) {
        std::cerr << "Server error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
