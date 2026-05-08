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
		int RandomOperationIndex = rand() % 4;
		sprintf_s(Message, "%d%c%d", FirstNumber, Operation[RandomOperationIndex], SecondNumber);

		int WantSendBytes = TOTAL_PACKET_SIZE;
		int SentBytes = 0;
		int TotalSentBytes = 0;
		do
		{
			SentBytes = send(ServerSocket, &Message[TotalSentBytes], WantSendBytes - TotalSentBytes, 0);
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
		int WantRecvBytes = TOTAL_PACKET_SIZE;
		int RecvBytes;

		RecvBytes = recv(ServerSocket, Buffer, WantRecvBytes, MSG_WAITALL);
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

		printf("%s=%s\n", Message, Buffer);
	}

	closesocket(ServerSocket);

	WSACleanup();
}