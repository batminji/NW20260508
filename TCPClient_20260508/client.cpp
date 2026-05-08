#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

int main()
{
	srand((USHORT)time(NULL));
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}

	SOCKET ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET)
	{
		std::cout << "Listen Socket Error" << std::endl;
		exit(-1);
	}

	SOCKADDR_IN ServerSocketAddr;
	memset(&ServerSocket, sizeof(ServerSocket), 0);
	ServerSocketAddr.sin_family = AF_INET;
	// ListenSocketAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (!inet_pton(AF_INET, "127.0.0.1", (PVOID)&ServerSocketAddr.sin_addr.s_addr))
	{
		std::cout << "inet_pton Error" << std::endl;
		exit(-1);
	}
	ServerSocketAddr.sin_port = htons(9000);

	connect(ServerSocket, (SOCKADDR*)&ServerSocketAddr, sizeof(ServerSocketAddr));

	char Message[1024] = { 0, };

	int FirstNumber = rand() % 99 + 1;
	int SecondNumber = rand() % 99 + 1;

	int SendBytes;
	SendBytes = send(ServerSocket, Message, sizeof(Message), 0);
	if (SendBytes == 0)
	{
	}
	else if (SendBytes < 0)
	{
		std::cout << "Send Error" << std::endl;
		exit(-1);
	}

	int RecvBytes;
	RecvBytes = recv(ServerSocket, Message, sizeof(Message), 0);
	if (RecvBytes == 0)
	{
		// Connection Closed
	}
	else if (RecvBytes < 0)
	{
		std::cout << "Recv Error" << std::endl;
		exit(-1);
	}

	WSACleanup();
}