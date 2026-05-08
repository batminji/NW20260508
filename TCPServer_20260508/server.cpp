#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

int main()
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}

	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET)
	{
		std::cout << "Listen Socket Error" << std::endl;
		exit(-1);
	}

	SOCKADDR_IN ListenSocketAddr;
	memset(&ListenSocket, sizeof(ListenSocket), 0);		// ZeroMemory(&ListenSocket, sizeof(ListenSocket), 0);
	ListenSocketAddr.sin_family = AF_INET;
	// ListenSocketAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if(!inet_pton(AF_INET, "127.0.0.1", (PVOID)&ListenSocketAddr.sin_addr.s_addr))
	{
		std::cout << "inet_pton Error" << std::endl;
		exit(-1);
	}
	ListenSocketAddr.sin_port = htons(9000);

	int retval;

	retval = bind(ListenSocket, (SOCKADDR*)&ListenSocketAddr, sizeof(ListenSocketAddr));
	if (retval == SOCKET_ERROR)
	{
		std::cout << "Bind Error" << std::endl;
		exit(-1);
	}

	retval = listen(ListenSocket, 0);
	if (retval == SOCKET_ERROR)
	{
		std::cout << "Listen Error" << std::endl;
		exit(-1);
	}

	while (true)
	{
		SOCKADDR_IN ClientSocketAddr;
		memset(&ClientSocketAddr, sizeof(ClientSocketAddr), 0);
		int ClientSocketAddrLength = sizeof(ClientSocketAddr);

		// Blocking
		SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSocketAddr, &ClientSocketAddrLength);
		if (ClientSocket == INVALID_SOCKET)
		{
			std::cout << "Client Socket Error" << std::endl;
			exit(-1);
		}

		char Buffer[1024] = { 0, };
		// Full Duplex Communication
		int RecvBytes;
		RecvBytes = recv(ClientSocket, Buffer, sizeof(Buffer), 0);		// OS TCP Buffer -> User Buffer
		if (RecvBytes == 0)
		{
			// Connection Closed
		}
		else if (RecvBytes < 0)
		{
			std::cout << "Recv Error" << std::endl;
			exit(-1);
		}

		// Packet Parsing
		// User Buffer -> OS TCP Buffer - Nagle Algorithm
		int SendBytes;
		SendBytes = send(ClientSocket, Buffer, RecvBytes, 0);
		if (SendBytes == 0)
		{
			// OS Buffer에 담지 못함.
		}
		else if (SendBytes < 0)
		{
			std::cout << "Send Error" << std::endl;
			exit(-1);
		}

		shutdown(ClientSocket, SD_BOTH);
		closesocket(ClientSocket);
	}
	closesocket(ListenSocket);

	WSACleanup();
}