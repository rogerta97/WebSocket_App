#pragma comment(lib, "ws2_32.lib") 

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define SOCKET_ERROR -1

#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std; 

enum SocketType {UDP, TCP};

struct Server
{
	SOCKET s;
	struct sockaddr_in bindAddr;
	SocketType connection_type; 
	
	string curr_message; 
	int num_connections; 
};

bool InitSockets()
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		// Log and handle error
		if (WSAGetLastError() == SOCKET_ERROR)
			return false;
	}

	return true;
}

bool CleanUpSockets()
{
	int iResult = WSACleanup();
	return true;
}

SOCKET CreateUDPSocket()
{
	return socket(AF_INET, SOCK_DGRAM, 0);
}

SOCKET CreateTCPSocket()
{
	return socket(AF_INET, SOCK_STREAM, 0);
}

bool CloseSocket(SOCKET s)
{
	auto result = closesocket(s);

	if (result == SOCKET_ERROR)
		return false;

	return true;
}

void printWSErrorAndExit(const char *msg)
{
	wchar_t *s = NULL;
	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s,
		0,
		NULL);
	fprintf(stderr, "%s: %S\n", msg, s);
	LocalFree(s);
	system("pause");
	exit(-1);
}

int main(int argc, char **argv)
{
	const char* error = "";
	Server server;

	InitSockets();

	int mode;

	printf("1.UDP\n2.TCP\n");
	printf("Select connection mode:");
	scanf_s("%d", &mode);

	if (mode - 1 == UDP)
	{
		server.s = CreateUDPSocket();
		server.connection_type = UDP;
	}
	else if (mode - 1 == TCP)
	{
		server.s = CreateTCPSocket();
		server.connection_type = TCP;
	}

	server.bindAddr.sin_family = AF_INET;
	server.bindAddr.sin_port = htons(8000);
	server.bindAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	server.num_connections = 0;

	int enable = 1;
	int ret = setsockopt(server.s, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));

	if (ret == SOCKET_ERROR)
		printWSErrorAndExit(error);

	ret = bind(server.s, (const struct sockaddr*)&server.bindAddr, sizeof(server.bindAddr));

	if (ret == SOCKET_ERROR)
		printWSErrorAndExit(error);

	if (server.connection_type == UDP)
	{
		

		while (server.num_connections < 5)
		{
			//Recieve Client Message
			char* buf = new char[10];
			int size = sizeof(sockaddr);
			ret = recvfrom(server.s, buf, 10, 0, (struct sockaddr*)&server.bindAddr, &size);

			server.curr_message = buf;

			if (server.curr_message == "")
				continue;

			//Send answer
			const char* message = "Pong";

			const char *remoteAddrStr = "127.0.0.1";
			inet_pton(AF_INET, remoteAddrStr, &server.bindAddr.sin_addr);

			ret = sendto(server.s, message, sizeof(message), 0, (struct sockaddr*)&server.bindAddr, sizeof(server.bindAddr));

			if (ret != false)
			{
				server.num_connections++;
				printf("Message %d sended succesfully (UDP): %s \n", server.num_connections, buf);
			}
		}
	}
	else
	{
		//Connect server & client 
		ret = listen(server.s, 1);

		if (ret == SOCKET_ERROR)
			printWSErrorAndExit(error);

		printf("Server status set to Listen Mode...\n");
		printf("Server is looking for connection...\n");

		int size = sizeof(sockaddr);
		ret = accept(server.s, (struct sockaddr*)&server.bindAddr, &size);

		if (ret == SOCKET_ERROR)
			printWSErrorAndExit(error);

		printf("Server is connected.\n");

		char* message = new char[10];

		while (server.num_connections < 5)
		{	
			ret = recv(server.s, message, (int)strlen(message), 0);

			if (ret == SOCKET_ERROR)
				printWSErrorAndExit(error);

			ret = send(server.s, "Pong", 10, 0);

			if (ret == SOCKET_ERROR)
				printWSErrorAndExit(error);

			server.num_connections++;
			printf("Message %d '%s' received and answered succesfully (TCP).", server.num_connections, message);
		}

	}
	printf("DONE SERVER!\n"); 
	system("pause"); 
	CloseSocket(server.s); 
	CleanUpSockets();
}


