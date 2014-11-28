//main.cpp

#include "vld.h"

#include <iostream>
#include <WinSock2.h>

#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")


int main(int arc, char* argv[])
{
	std::cout << std::endl;

	WSADATA data;
	SOCKET listener, new_socket;
	struct sockaddr_in server, client;
	int size;
	char* message;


	std::cout << "Initializing winsock... ";
	if (WSAStartup(MAKEWORD(2, 1), &data) != 0)
	{
		std::cout << "Failed! Error: " << WSAGetLastError() << std::endl;
		return 1;
	}
	std::cout << "done!" << std::endl;


	std::cout << "Creating socket... ";
	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		std::cout << "Failed! Error: " << WSAGetLastError() << std::endl;
	}
	std::cout << "done!" << std::endl;


	//prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.S_un.S_addr = INADDR_ANY; // <-- ?
	server.sin_port = htons( 8888 );


	std::cout << "Binding socket... ";
	if (bind(listener, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		std::cout << "Failed! Error: " << WSAGetLastError() << std::endl;
	}
	std::cout << "done!" << std::endl;


	std::cout << "Listening..." << std::endl;
	listen(listener, SOMAXCONN);


	//Accept incomming connection
	size = sizeof(struct sockaddr_in);
	new_socket = accept(listener, (struct sockaddr*)&client, &size);
	if (new_socket == INVALID_SOCKET)
	{
		std::cout << "Failed to accept new connection! Error: " << WSAGetLastError() << std::endl;
	}
	std::cout << "New connection accepted!" << std::endl;


	//Reply
	message = "Hello client! Your connection has been recieved. It is now aborted, goodbye! \n";
	send(new_socket, message, strlen(message), 0);


	getchar();


	closesocket(listener);
	WSACleanup();

	std::cin.get();
	return 0;
}