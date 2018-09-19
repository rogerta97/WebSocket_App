#pragma comment(lib, "ws2_32.lib") 

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define SOCKET_ERROR -1

#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

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

int main(int argc, char **argv)
{
	InitSockets(); 
	SOCKET s = CreateUDPSocket(); 

	struct sockaddr_in bindAddr;
	bindAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	int bind_result = bind(s, (const struct sockaddr*)&bindAddr, sizeof(bindAddr));

	CloseSocket(s); 
	CleanUpSockets();
}

