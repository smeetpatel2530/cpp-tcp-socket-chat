# C++ TCP Socket Chat System

<p align="center">
  <b>Multi-client chat application built with C++17, TCP/IP sockets, and multithreading</b><br>
  Cross-platform support for <b>Windows (Winsock2)</b> and <b>Linux (POSIX sockets)</b>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg" alt="C++17">
  <img src="https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-green.svg" alt="Platform">
  <img src="https://img.shields.io/badge/Networking-TCP%2FIP-orange.svg" alt="TCP/IP">
  <img src="https://img.shields.io/badge/Concurrency-Multithreading-purple.svg" alt="Multithreading">
  <img src="https://img.shields.io/badge/Build-CMake-red.svg" alt="CMake">
</p>

---

## Overview

This project is a production-style **multi-client TCP chat system** developed in **C++17** to demonstrate practical knowledge of:

- **TCP/IP socket programming**
- **Client-server architecture**
- **Multithreading and concurrency**
- **Cross-platform systems development**
- **Structured logging and debugging**
- **Modular software design**


---

## Why this project stands out

Unlike a basic single-user socket demo, this project supports:

- Multiple clients connecting to one server
- Real-time message broadcasting
- Username-based join and leave notifications
- Thread-per-client communication handling
- Logging for easier debugging and observability
- Cross-platform support using:
  - **Winsock2** on Windows
  - **POSIX sockets** on Linux/macOS

This makes the project much closer to a real-world networking/system software mini-build than a simple classroom implementation.

---

## Key Features

- Multi-client TCP chat server
- Real-time client-server communication
- Username-based session handling
- `/list` command to display active users
- `/quit` command for graceful disconnect
- Broadcast messaging across connected clients
- Timestamped logging to `.log` files
- Cross-platform socket abstraction
- Clean folder structure and modular code organization
- Build support via **g++** and **CMake**
- GitHub Actions CI workflow

---

## Tech Stack

| Area | Technology |
|------|------------|
| Language | C++17 |
| Networking | Winsock2 (Windows), POSIX sockets (Linux/macOS) |
| Concurrency | `std::thread`, `std::mutex` |
| Build | CMake, g++ |
| Logging | File-based timestamped logs |
| Platform | Windows, Linux, macOS |

---

## Architecture

### High-Level Flow

```text
+------------------+        TCP Connection        +------------------+
|   Client 1       |  ------------------------->  |                  |
| (chat_client)    |                              |                  |
+------------------+                              |                  |
                                                  |                  |
+------------------+        TCP Connection        |    Chat Server   |
|   Client 2       |  ------------------------->  |   (chat_server)  |
| (chat_client)    |                              |                  |
+------------------+                              |                  |
                                                  |                  |
+------------------+        TCP Connection        |                  |
|   Client N       |  ------------------------->  |                  |
| (chat_client)    |                              +------------------+
+------------------+
```

### Internal Design

- **`server_main.cpp`** starts the server on a configurable port.
- **`chat_server.cpp`** manages connections, receives messages, and broadcasts them.
- **`client_main.cpp`** starts a client with host, port, and username.
- **`chat_client.cpp`** handles sending input and receiving server messages concurrently.
- **`platform.hpp`** provides cross-platform socket compatibility.
- **`logger.cpp`** writes timestamped logs for debugging and tracking activity.

---

## Project Structure

```bash
cpp-tcp-socket-chat/
├── include/
│   ├── platform.hpp
│   ├── chat_server.hpp
│   ├── chat_client.hpp
│   ├── logger.hpp
│   └── utils.hpp
├── src/
│   ├── server_main.cpp
│   ├── client_main.cpp
│   ├── chat_server.cpp
│   ├── chat_client.cpp
│   ├── logger.cpp
│   └── utils.cpp
├── screenshots/
│   ├── 1socket.png
│   ├── 2socket.png
│   └── 3socket.png
├── .github/
│   └── workflows/
│       └── ci.yml
├── CMakeLists.txt
└── README.md
```

---

## Build Instructions

### Windows (MSYS2 / g++)

```bash
g++ -std=c++17 -Iinclude src/server_main.cpp src/chat_server.cpp src/logger.cpp src/utils.cpp -o chat_server.exe -lws2_32 -pthread

g++ -std=c++17 -Iinclude src/client_main.cpp src/chat_client.cpp src/logger.cpp src/utils.cpp -o chat_client.exe -lws2_32 -pthread
```

### Linux / macOS

```bash
g++ -std=c++17 -Iinclude src/server_main.cpp src/chat_server.cpp src/logger.cpp src/utils.cpp -o chat_server -pthread

g++ -std=c++17 -Iinclude src/client_main.cpp src/chat_client.cpp src/logger.cpp src/utils.cpp -o chat_client -pthread
```

---

## Quick Start

### Terminal 1 — Start Server

```bash
.\chat_server.exe 9090
```

### Terminal 2 — Start Client 1

```bash
.\chat_client.exe 127.0.0.1 9090 smeet
```

### Terminal 3 — Start Client 2

```bash
.\chat_client.exe 127.0.0.1 9090 guest2
```

---

## Supported Commands

| Command | Description |
|---------|-------------|
| `/list` | Shows currently connected users |
| `/quit` | Gracefully disconnects from the chat |
| any text | Sends a broadcast message to connected clients |

---

## Demo Output

### Terminal 1 — Server

```bash
Starting Chat Server on port 9090...
[2026-05-23 14:47:21] [INFO ] Server listening on port 9090
[2026-05-23 14:47:21] [INFO ] Waiting for clients...
[2026-05-23 14:47:44] [INFO ] New connection from 127.0.0.1
[2026-05-23 14:47:44] [INFO ] smeet joined from 127.0.0.1
[2026-05-23 14:48:05] [INFO ] New connection from 127.0.0.1
[2026-05-23 14:48:05] [INFO ] guest2 joined from 127.0.0.1
```

### Terminal 2 — Client 1

```bash
[2026-05-23 14:47:44] [INFO ] Connected to 127.0.0.1:9090 as smeet
=== Connected as [smeet] ===
Commands: /list  /quit  or type any message
[server] Welcome, smeet!
[server] guest2 joined from 127.0.0.1
```

### Terminal 3 — Client 2

```bash
[2026-05-23 14:48:05] [INFO ] Connected to 127.0.0.1:9090 as guest2
=== Connected as [guest2] ===
Commands: /list  /quit  or type any message
[server] Welcome, guest2!
```

---

## Screenshots

### Terminal 1 — Server View

![Terminal 1 - Server](screenshots/1socket.png)

### Terminal 2 — Client 1 View

![Terminal 2 - Client 1](screenshots/2socket.png)

### Terminal 3 — Client 2 View

![Terminal 3 - Client 2](screenshots/3socket.png)

---

## Engineering Highlights

This project demonstrates:

- **Socket programming** using TCP
- **Systems-level C++ development**
- **Cross-platform networking compatibility**
- **Concurrent client handling**
- **Practical debugging through logs**
- **Reusable modular code structure**
- **Command-driven terminal application design**


---


## Future Improvements

- Private messaging with `/msg <username> <message>`
- File transfer between clients
- Persistent chat history
- User authentication
- Dockerized execution
- Event-driven scalability using `select()` or `epoll`
- Unit and integration test coverage
---


## Author

**Smeet Patel**

- GitHub: [smeetpatel2530](https://github.com/smeetpatel2530)
- LinkedIn: [Smeet Patel](https://www.linkedin.com/in/smeet-patel-22b67a193/)

---

## License

This project is open for learning and portfolio use. Add an MIT License if you want to make it formally reusable.
