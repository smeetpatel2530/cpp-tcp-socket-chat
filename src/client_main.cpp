#include "chat_client.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    std::string host = "127.0.0.1";
    int port = 9090;
    std::string username = "user";

    if (argc > 1) host = argv[1];
    if (argc > 2) port = std::stoi(argv[2]);
    if (argc > 3) username = argv[3];

    try {
        ChatClient client(host, port, username, "client.log");
        client.run();
    } catch (const std::exception& ex) {
        std::cerr << "Client error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
