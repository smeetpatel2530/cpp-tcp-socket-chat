# C++ TCP Socket Chat System

A production-style **client-server chat application** built with **C++17**, **POSIX sockets**, and **multithreading**. This project is tailored for **Ciena-style software/networking roles** because it demonstrates TCP/IP communication, concurrency, Linux-friendly development, structured logging, modular code, and clean engineering practices.

## Features

- Multi-client TCP chat server
- Username-based join/leave notifications
- Broadcast messaging
- `/list` command to view active users
- `/quit` command to exit gracefully
- Thread-per-client handling
- Structured logging to `server.log` and `client.log`
- CMake-based build
- GitHub Actions CI workflow

## Project Structure

```bash
cpp_tcp_socket_chat/
├── CMakeLists.txt
├── README.md
├── assets/
├── build/
├── include/
│   ├── chat_client.hpp
│   ├── chat_server.hpp
│   ├── logger.hpp
│   └── utils.hpp
├── screenshots/
│   └── README.txt
├── src/
│   ├── chat_client.cpp
│   ├── chat_server.cpp
│   ├── client_main.cpp
│   ├── logger.cpp
│   ├── server_main.cpp
│   └── utils.cpp
└── .github/
    └── workflows/
        └── ci.yml
```

## Build

### Using CMake
```bash
mkdir build && cd build
cmake ..
make
```

### Using g++
```bash
g++ -std=c++17 -Iinclude src/server_main.cpp src/chat_server.cpp src/logger.cpp src/utils.cpp -o chat_server -pthread
g++ -std=c++17 -Iinclude src/client_main.cpp src/chat_client.cpp src/logger.cpp src/utils.cpp -o chat_client -pthread
```

## Run

### Terminal 1
```bash
./chat_server 9090
```

### Terminal 2
```bash
./chat_client 127.0.0.1 9090 smeet
```

### Terminal 3
```bash
./chat_client 127.0.0.1 9090 guest2
```

## Commands

- Type any message and press Enter to broadcast.
- `/list` shows active users.
- `/quit` exits the client.

