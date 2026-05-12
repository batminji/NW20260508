#include "stdafx.h"

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
	memset(&ListenSocketAddr, 0, sizeof(ListenSocketAddr));
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
		memset(&ClientSocketAddr, 0, sizeof(ClientSocketAddr));
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
			PacketHeader RecvHeader;

			char Buffer[1024] = { 0, };
			// Full Duplex Communication
			int RecvBytes;
			int WantRecvBytes = sizeof(RecvHeader);
			RecvBytes = recv(ClientSocket, (char*)&RecvHeader, WantRecvBytes, MSG_WAITALL);
			if (RecvBytes == 0)
			{
				// Connection Closed
				printf("Client Connection Closed\n");
				break;
			}
			else if (RecvBytes < 0)
			{
				printf("Recv Error\n");
				break;
			}

			RecvHeader.Size = ntohs(RecvHeader.Size);
			RecvHeader.PacketType = ntohs(RecvHeader.PacketType);

			long long Result = 0;

			switch (static_cast<EPacketType>(RecvHeader.PacketType))
			{
			case EPacketType::CS_Calculate:
			{
				CS_Calculate RecvData;
				RecvBytes = recv(ClientSocket, (char*)&RecvData, RecvHeader.Size, MSG_WAITALL);
				if (RecvBytes == 0)
				{
					// Connection Closed
					printf("Client Connection Closed\n");
					break;
				}
				else if (RecvBytes < 0)
				{
					printf("Recv Error\n");
					break;
				}

				RecvData.FirstNumber = ntohl(RecvData.FirstNumber);
				RecvData.SecondNumber = ntohl(RecvData.SecondNumber);
				RecvData.OperationType = ntohs(RecvData.OperationType);

				switch (static_cast<EOperationType>(RecvData.OperationType))
				{
				case EOperationType::Plus:		
					Result = RecvData.FirstNumber + RecvData.SecondNumber; 
					break;
				case EOperationType::Minus:		
					Result = RecvData.FirstNumber - RecvData.SecondNumber; 
					break;
				case EOperationType::Multiply:	
					Result = (long long)RecvData.FirstNumber * RecvData.SecondNumber; 
					break;
				case EOperationType::Divide:
					Result = RecvData.FirstNumber / RecvData.SecondNumber;
					break;
				}
			}
			break;
			default:
				printf("Unknown Packet Code\n");
				break;
			}

			// Send
			// Packet Parsing
			// User Buffer -> OS TCP Buffer - Nagle Algorithm
			PacketHeader SendHeader;
			SC_Calculate SendData;

			SendData.Result = htonll(Result); // 엔디안 변환

			SendHeader.Size = htons(sizeof(SC_Calculate));
			SendHeader.PacketType = htons(static_cast<unsigned short>(EPacketType::SC_Calculate));

			send(ClientSocket, (char*)&SendHeader, sizeof(SendHeader), 0);

			send(ClientSocket, (char*)&SendData, sizeof(SC_Calculate), 0);
		}
		shutdown(ClientSocket, SD_BOTH);
		closesocket(ClientSocket);
	}
	closesocket(ListenSocket);

	WSACleanup();
}