#include "stdafx.h"

int main()
{
	srand((unsigned int)time(nullptr));
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}

	hostent* HostInfo = gethostbyname("login.calculate.edu");

	char ServerIP[1024] = { 0, };
	IN_ADDR Addr;
	Addr.s_addr = *(ULONG*)*HostInfo->h_addr_list;
	sprintf_s(ServerIP, "%s", inet_ntoa(Addr));
	printf("%s\n", ServerIP);

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
	if (!inet_pton(AF_INET, ServerIP, (PVOID)&ServerSocketAddr.sin_addr.s_addr))
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
		int FirstNumber = rand() % RANDOM_NUMBER_MAX + 1;
		int SecondNumber = rand() % RANDOM_NUMBER_MAX + 1;
		unsigned short RandomOperationIndex = rand() % 4;

		printf("%d%c%d", FirstNumber, Operation[RandomOperationIndex], SecondNumber);

		PacketHeader SendHeader;
		CS_Calculate SendData;

		SendData.FirstNumber = htonl(FirstNumber);
		SendData.SecondNumber = htonl(SecondNumber);
		SendData.OperationType = htons(RandomOperationIndex);

		SendHeader.Size = htons(sizeof(CS_Calculate));
		SendHeader.PacketType = htons(static_cast<unsigned short>(EPacketType::CS_Calculate));

		// Header
		int WantSendBytes = sizeof(SendHeader);
		int SentBytes = 0;
		int TotalSentBytes = 0;
		do
		{
			SentBytes = send(ServerSocket, (char*)&SendHeader + TotalSentBytes, WantSendBytes - TotalSentBytes, 0);
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
		WantSendBytes = ntohs(SendHeader.Size);
		SentBytes = 0;
		TotalSentBytes = 0;
		do
		{
			SentBytes = send(ServerSocket, (char*)&SendData + TotalSentBytes, WantSendBytes - TotalSentBytes, 0);
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

		PacketHeader RecvHeader;
		int WantRecvBytes = sizeof(RecvHeader);
		int RecvBytes = 0;
		int TotalRecvBytes = 0;
		do
		{
			RecvBytes = recv(ServerSocket, (char*)&RecvHeader + TotalRecvBytes, WantRecvBytes - TotalRecvBytes, MSG_WAITALL);
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
			TotalRecvBytes += RecvBytes;
		} while (TotalRecvBytes < WantRecvBytes);

		RecvHeader.Size = ntohs(RecvHeader.Size);
		RecvHeader.PacketType = ntohs(RecvHeader.PacketType);

		switch (static_cast<EPacketType>(RecvHeader.PacketType))
		{
		case EPacketType::SC_Calculate:
		{
			SC_Calculate RecvData;
			WantRecvBytes = RecvHeader.Size;
			RecvBytes = 0;
			TotalRecvBytes = 0;
			do
			{
				RecvBytes = recv(ServerSocket, (char*)&RecvData + TotalRecvBytes, WantRecvBytes - TotalRecvBytes, MSG_WAITALL);
				if (RecvBytes <= 0)
				{
					printf("Connection Closed or Recv Error\n");
					exit(-1);
				}
				TotalRecvBytes += RecvBytes;
			} while (TotalRecvBytes < WantRecvBytes);

			long long Result = ntohll(RecvData.Result);
			printf("= %lld\n", Result);
		}
		break;

		default:
			printf("Unknown or Invalid Packet Code: %d\n", RecvHeader.PacketType);
			exit(-1);
			break;
		}
	}

	closesocket(ServerSocket);

	WSACleanup();
}