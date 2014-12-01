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
	//Init
	WSADATA data;
	SOCKET listener;
	struct sockaddr_in server;

	server.sin_family = AF_INET;
	server.sin_addr.S_un.S_addr = INADDR_ANY; // <-- ?
	server.sin_port = htons(8080);

	std::cout << "Initializing winsock... ";
	if (WSAStartup(MAKEWORD(2, 1), &data) != 0)
	{
		std::cout << "Failed! Error: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}
	std::cout << "done!" << std::endl;


	//--------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------


	//Listener Socket
	std::cout << "Creating socket... ";
	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		std::cout << "Failed! Error: " << WSAGetLastError() << std::endl;
	}
	std::cout << "done!" << std::endl;


	std::cout << "Binding socket... ";
	if (bind(listener, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		std::cout << "Failed! Error: " << WSAGetLastError() << std::endl;
	}
	std::cout << "done!" << std::endl;


	//--------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------


	//Listening
	std::cout << "Listening..." << std::endl;
	listen(listener, SOMAXCONN);


	WSAEVENT wsa_event = WSACreateEvent();
	WSAEventSelect(listener, wsa_event, FD_ACCEPT);


	while (true)
	{
		SOCKET new_socket;
		struct sockaddr_in remote;
		int size = sizeof(struct sockaddr_in);

		char* message;

		if (WSAWaitForMultipleEvents(1, &wsa_event, TRUE, 10, FALSE) == WSA_WAIT_EVENT_0)
		{
			new_socket = accept(listener, (sockaddr*)&remote, &size);

			if (new_socket != INVALID_SOCKET)
			{
				std::cout << "New connection accepted!" << std::endl;

				//Reply
				message = "Hello client! Your connection has been recieved. It is now aborted, goodbye! \n";
				send(new_socket, message, strlen(message), 0);
			}
		}
	}
	




	//Close
	WSACloseEvent(wsa_event);
	closesocket(listener);
	WSACleanup();
	return 0;
}


/*
	OLD CODE:

	//Accept incomming connection
	size = sizeof(struct sockaddr_in);
	while ((new_socket = accept(listener, (struct sockaddr*)&client, &size)) != INVALID_SOCKET)
	{
	std::cout << "New connection accepted!" << std::endl;

	//Reply
	message = "Hello client! Your connection has been recieved. It is now aborted, goodbye! \n";
	send(new_socket, message, strlen(message), 0);
	}


	if (new_socket == INVALID_SOCKET)
	{
	std::cout << "Failed to accept new connection! Error: " << WSAGetLastError() << std::endl;
	closesocket(listener);
	WSACleanup();
	return 1;
	}



*/