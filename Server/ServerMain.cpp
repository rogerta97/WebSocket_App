#pragma comment(lib, "ws2_32.lib") 

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define SOCKET_ERROR -1

#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <stdio.h>
#include <stdlib.h>

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

	InitSockets(); 
	SOCKET s = CreateUDPSocket(); 

	struct sockaddr_in bindAddr;
	bindAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	int enable = 1;

	int setsock_result = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));

	if (setsock_result == SOCKET_ERROR)
		printWSErrorAndExit(error);

	int bind_result = bind(s, (const struct sockaddr*)&bindAddr, sizeof(bindAddr));
	
	if (bind_result == SOCKET_ERROR)
		printWSErrorAndExit(error);
	
	CloseSocket(s); 
	CleanUpSockets();
}

