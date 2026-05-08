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
	memset(&ListenSocket, sizeof(ListenSocket), 0);
	ListenSocketAddr.sin_family = AF_INET;
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

		while (true)
		{
			char Buffer[1024] = { 0, };
			// Full Duplex Communication
			int RecvBytes;
			int WantRecvBytes = 5;
			RecvBytes = recv(ClientSocket, Buffer, WantRecvBytes, MSG_WAITALL);		// OS TCP Buffer -> User Buffer
			if (RecvBytes == 0)
			{
				// Connection Closed
			}
			else if (RecvBytes < 0)
			{
				std::cout << "Recv Error" << std::endl;
				exit(-1);
			}

			// Process
			std::string Packet(Buffer);
			int OperatiorIndex = static_cast<int>(Packet.find('+'));
			std::string FirstStringNumber = Packet.substr(0, OperatiorIndex);
			std::string SecondStringNumber = Packet.substr(OperatiorIndex + 1, 2);
			int FirstNumber = atoi(FirstStringNumber.c_str());
			int SecondNumber = atoi(SecondStringNumber.c_str());

			// printf("%s + %s = %d\n", FistNumber.c_str(), SecondNumber.c_str(), atoi(FistNumber.c_str()) + atoi(SecondNumber.c_str()));

			// Packet Parsing
			// User Buffer -> OS TCP Buffer - Nagle Algorithm
			char Message[1024] = { 0, };
			int WantSendBytes = 5;
			int SentBytes = 0;
			int TotalSentBytes = 0;

			sprintf_s(Message, "%d", FirstNumber + SecondNumber);

			do
			{
				SentBytes = send(ClientSocket, &Message[TotalSentBytes], WantSendBytes - TotalSentBytes, 0);
				if (SentBytes == 0)
				{
					printf("Connection Closed\n");
					exit(-1);
				}
				else if (SentBytes < 0)
				{
					printf("Send Error\n");
					exit(-1);
				}
				TotalSentBytes += SentBytes;
			} while (TotalSentBytes < WantSendBytes);
		}
		shutdown(ClientSocket, SD_BOTH);
		closesocket(ClientSocket);
	}
	closesocket(ListenSocket);

	WSACleanup();
}