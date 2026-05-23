cmake_minimum_required(VERSION 3.16)
project(ciena_socket_chat LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include_directories(include)

add_executable(chat_server
    src/server_main.cpp
    src/chat_server.cpp
    src/logger.cpp
    src/utils.cpp
)
target_link_libraries(chat_server ws2_32 pthread)

add_executable(chat_client
    src/client_main.cpp
    src/chat_client.cpp
    src/logger.cpp
    src/utils.cpp
)
target_link_libraries(chat_client ws2_32 pthread)
