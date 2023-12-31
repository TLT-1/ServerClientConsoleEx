// ServerClientConsole.cpp 
// By Phillip Croff
// message format is 1-byte for the message length followed by the message
//  ex:   6hello\0

#define _CRT_SECURE_NO_WARNINGS                 // turns of deprecated warnings
#define _WINSOCK_DEPRECATED_NO_WARNINGS         // turns of deprecated warnings for winsock
#include <winsock2.h>
//#include <ws2tcpip.h>                         // only need if you use inet_pton
#pragma comment(lib,"Ws2_32.lib")

#include <iostream>

int tcp_recv_whole(SOCKET s, char* buf, int len);
int tcp_send_whole(SOCKET skSocket, const char *data, uint16_t length);
void ServerCode(void);
void ClientCode(void);


int main()
{
	WSADATA wsadata;
	WSAStartup(WINSOCK_VERSION, &wsadata);

	int choice;
	do
	{
		printf("Would you like to Create a Server or Client?\n");
		printf("1> Server\n");
		printf("2> Client\n");
		std::cin >> choice;
	} while (choice != 1 && choice != 2);

	//Server
	if (choice == 1)
	{
		ServerCode();
	}

	//Client
	if (choice == 2)
	{
		ClientCode();
	}

	return WSACleanup();
}
// loop recv func
int tcp_recv_whole(SOCKET s, char* buf, int len)
{
	int total = 0;

	do
	{
		int ret = recv(s, buf + total, len - total, 0);
		if (ret < 1)
			return ret;
		else
			total += ret;

	} while (total < len);

	return total;
}

// loop send func
int tcp_send_whole(SOCKET skSocket, const char *data, uint16_t length)
{
	int result;
	int bytesSent = 0;

	while (bytesSent < length)
	{
		result = send(skSocket, (const char *)data + bytesSent, length - bytesSent, 0);

		if (result <= 0)
			return result;

		bytesSent += result;
	}

	return bytesSent;
}

void ServerCode(void)
{
	//Socket
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("DEBUG// Socket function incorrect\n");
		return;
	}
	else
	{
		printf("DEBUG// I used the socket function\n");
	}

	//Bind
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	serverAddr.sin_port = htons(31337);

	int result = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR)
	{
		printf("DEBUG// Bind function incorrect\n");
		return;
	}
	else
	{
		printf("DEBUG// I used the bind function\n");
	}

	//Listen
	result = listen(listenSocket, 1);
	if (result == SOCKET_ERROR)
	{
		printf("DEBUG// Listen function incorrect\n");
		return;
	}
	else
	{
		printf("DEBUG// I used the listen function\n");
	}

	printf("Waiting...\n\n");

	SOCKET ComSocket = accept(listenSocket, NULL, NULL);
	if (ComSocket == INVALID_SOCKET)
	{
		printf("DEBUG// Accept function incorrect\n");
		return;

	}
	else
	{
		printf("DEBUG// I used the accept function\n");
	}

	//Communication
	uint8_t size = 0;

	result = tcp_recv_whole(ComSocket, (char*)&size, 1);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		printf("DEBUG// recv is incorrect\n");
		return;

	}
	else
	{
		printf("DEBUG// I used the recv function\n");
	}

	char * buffer = new char[size];

	result = tcp_recv_whole(ComSocket, (char*)buffer, size);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		printf("DEBUG// recv is incorrect\n");
		return;

	}
	else
	{
		printf("DEBUG// I used the recv function\n");
	}

	printf("DEBUG// I received a message from the client\n");

	printf("\n\n");
	printf(buffer);
	printf("\n\n");

	delete[] buffer;

	// close both sockets
	shutdown(listenSocket, SD_BOTH);
	closesocket(listenSocket);

	shutdown(ComSocket, SD_BOTH);
	closesocket(ComSocket);
}

void ClientCode(void)
{
	//Socket
	SOCKET ComSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ComSocket == INVALID_SOCKET)
	{
		printf("DEBUG// Socket function incorrect\n");
		return;
	}
	else
	{
		printf("DEBUG// I used the socket function\n");
	}

	//Connect
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	serverAddr.sin_port = htons(31337);

	int result = connect(ComSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR)
	{
		printf("DEBUG// Connect function incorrect\n");
		return;
	}
	else
	{
		printf("DEBUG// I used the Connect function\n");
	}

	//Communication
	uint8_t size = 30;
	char sendbuffer[30];
	memset(sendbuffer, 0, 30);
	strcpy(sendbuffer, "I'm a message from the client");

	result = tcp_send_whole(ComSocket, (char*)&size, 1);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		printf("DEBUG// send is incorrect\n");
		return;

	}
	else
	{
		printf("DEBUG// I used the send function\n");
	}



	result = tcp_send_whole(ComSocket, sendbuffer, size);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		printf("DEBUG// send is incorrect\n");
		return;

	}
	else
	{
		printf("DEBUG// I used the send function\n");
	}

	printf("DEBUG// I sent a message to the server\n");
	

	// close sockets
	shutdown(ComSocket, SD_BOTH);
	closesocket(ComSocket);
}
