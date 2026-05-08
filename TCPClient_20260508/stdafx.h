#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "protocol.h"

#pragma comment(lib, "ws2_32")

#define RANDOM_NUMBER_MAX 9999
constexpr char Operation[4] = { '+', '-', '*', '/' };