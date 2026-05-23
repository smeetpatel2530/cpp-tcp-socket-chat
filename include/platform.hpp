#pragma once

#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "ws2_32.lib")
  using socket_t = SOCKET;
  #define INVALID_SOCK INVALID_SOCKET
  #define close_sock(s) closesocket(s)
  #define sock_error() WSAGetLastError()
  // Windows has no MSG_NOSIGNAL
  #ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0
  #endif
#else
  #include <arpa/inet.h>
  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <unistd.h>
  #include <netdb.h>
  using socket_t = int;
  #define INVALID_SOCK (-1)
  #define close_sock(s) close(s)
  #define sock_error() errno
#endif
