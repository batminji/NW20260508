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
			PacketHeader Header;

			char Buffer[1024] = { 0, };
			// Full Duplex Communication
			int RecvBytes;
			int WantRecvBytes = TOTAL_HEADER_SIZE;
			RecvBytes = recv(ClientSocket, (char*)&Header, WantRecvBytes, MSG_WAITALL);
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

			Header.Size = ntohs(Header.Size);
			Header.Code = ntohs(Header.Code);

			// Process
			TwoNumber Data;
			RecvBytes = recv(ClientSocket, (char*)&Data, Header.Size, MSG_WAITALL);
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

			Data.FirstNumber = ntohs(Data.FirstNumber);
			Data.SecondNumber = ntohs(Data.SecondNumber);

			long long Result = 0;
			switch (static_cast<EPacketType>(Header.Code))
			{
			case EPacketType::Plus:				
				Result = Data.FirstNumber + Data.SecondNumber;
				break;
			case EPacketType::Minus:
				Result = Data.FirstNumber - Data.SecondNumber;
				break;
			case EPacketType::Multiply:
				Result = Data.FirstNumber * Data.SecondNumber;
				break;
			case EPacketType::Divide:
				Result = Data.FirstNumber / Data.SecondNumber;
				break;
			}

			// Send
			// Packet Parsing
			// User Buffer -> OS TCP Buffer - Nagle Algorithm
			char Message[1024] = { 0, };
			int WantSendBytes = TOTAL_PACKET_SIZE;
			int SentBytes = 0;
			int TotalSentBytes = 0;

			PacketHeader SendHeader;
			SendHeader.Size = htons(sizeof(Result));
			SendHeader.Code = htons(static_cast<unsigned short>(EPacketType::Result));

			do
			{
				SentBytes = send(ClientSocket, (char*)&SendHeader + TotalSentBytes, WantSendBytes - TotalSentBytes, 0);
				if (SentBytes <= 0)
				{
					printf("Client Connection Closed\n");
					break;
				}
				TotalSentBytes += SentBytes;
			} while (TotalSentBytes < WantSendBytes);

			Result = htonll(Result);
			sprintf_s(Message, "%lld", Result);

			WantSendBytes = sizeof(Result);
			SentBytes = 0;
			TotalSentBytes = 0;

			do
			{
				SentBytes = send(ClientSocket, (char*)&Result + TotalSentBytes, WantSendBytes - TotalSentBytes, 0);
				if (SentBytes <= 0)
				{
					printf("Client Connection Closed\n");
					break;
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