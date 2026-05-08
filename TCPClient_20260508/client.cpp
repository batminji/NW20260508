#include "stdafx.h"

int main()
{
	srand((unsigned int)time(nullptr));
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
	memset(&ServerSocketAddr, 0, sizeof(ServerSocketAddr));
	ServerSocketAddr.sin_family = AF_INET;
	// ListenSocketAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (!inet_pton(AF_INET, "127.0.0.1", (PVOID)&ServerSocketAddr.sin_addr.s_addr))
	{
		printf("inet_pton Error\n");
		exit(-1);
	}
	ServerSocketAddr.sin_port = htons(9000);

	int retval;
	retval = connect(ServerSocket, (SOCKADDR*)&ServerSocketAddr, sizeof(ServerSocketAddr));
	if(retval == SOCKET_ERROR)
	{
		printf("Connect Error\n");
		exit(-1);
	}

	while (true)
	{
		char Message[1024] = { 0, };

		int FirstNumber = rand() % 9999 + 1;
		int SecondNumber = rand() % 9999 + 1;
		unsigned short RandomOperationIndex = rand() % 4;

		PacketHeader Header;
		Header.Size = 0;
		Header.Code = static_cast<unsigned short>(RandomOperationIndex);

		int Temp = htonl(FirstNumber);
		int DataCursor = 0;
		memcpy(&Message[DataCursor], &Temp, sizeof(int));
		DataCursor += sizeof(int);

		Temp = htonl(SecondNumber);
		memcpy(&Message[DataCursor], &Temp, sizeof(int));
		DataCursor += sizeof(int);

		Header.Size = DataCursor;

		Header.Size = htons(Header.Size);
		Header.Code = htons(Header.Code);

		// sprintf_s(Message, "%d%c%d", FirstNumber, Operation[RandomOperationIndex], SecondNumber);

		// Header
		int WantSendBytes = TOTAL_HEADER_SIZE;
		int SentBytes = 0;
		int TotalSentBytes = 0;
		do
		{
			SentBytes = send(ServerSocket, (char*)&Header + TotalSentBytes, WantSendBytes - TotalSentBytes, 0);
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

		// Data
		WantSendBytes = DataCursor;
		SentBytes = 0;
		TotalSentBytes = 0;
		do
		{
			SentBytes = send(ServerSocket, (char*)&Message[TotalSentBytes], WantSendBytes - TotalSentBytes, 0);
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

		char Buffer[1024] = { 0, };
		int WantRecvBytes = TOTAL_HEADER_SIZE;
		int RecvBytes;

		PacketHeader RecvHeader;

		RecvBytes = recv(ServerSocket, (char*)&RecvHeader, WantRecvBytes, MSG_WAITALL);
		if (RecvBytes == 0)
		{
			printf("Connection Closed\n");
			exit(-1);
		}
		else if (RecvBytes < 0)
		{
			printf("Recv Error\n");
			exit(-1);
		}

		RecvHeader.Size = ntohs(RecvHeader.Size);
		RecvHeader.Code = ntohs(RecvHeader.Code);

		if(RecvHeader.Code != static_cast<unsigned short>(EPacketType::Result))
		{
			printf("Invalid Packet Code\n");
			exit(-1);
		}

		WantRecvBytes = RecvHeader.Size;
		RecvBytes;

		RecvBytes = recv(ServerSocket, (char*)&Buffer, WantRecvBytes, MSG_WAITALL);
		if (RecvBytes == 0)
		{
			printf("Connection Closed\n");
			exit(-1);
		}
		else if (RecvBytes < 0)
		{
			printf("Recv Error\n");
			exit(-1);
		}

		long long Result = 0;
		memcpy(&Result, Buffer, sizeof(long long));
		Result = ntohll(Result);

		printf("%s=%s\n", Message, Buffer);
	}

	closesocket(ServerSocket);

	WSACleanup();
}