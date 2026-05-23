#pragma once
#include <string>
#include "platform.hpp"

std::string now_timestamp();
std::string trim(const std::string& s);


void init_sockets();
void cleanup_sockets();
