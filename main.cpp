//main.cpp

#include "vld.h"

#include <fstream>
#include <iostream>
#include <WinSock2.h>

#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

enum HTTPcode
{
	HTTPCODE_OK = 200,
	HTTPCODE_REFUSE = 403,
	HTTPCODE_NOTFOUND = 404,
};

enum HTTPcontent
{
	HTTP_HTML,
	HTTP_NONE,
};

struct Header
{
	Header(const HTTPcode &code, const HTTPcontent &type)
	{
		m_code = code;
		m_type = type;
	}

	HTTPcode m_code;
	HTTPcontent m_type;
};



struct HTMLPage
{
	HTMLPage(const std::string &name, const std::string data)
	{
		m_fileName = name;
		m_fileData = data;
	}

	std::string m_fileName;
	std::string m_fileData;
};



void HTMLSendPage(const SOCKET sock)
{
	std::string input = "";

	std::ifstream openfile("../WebPage/index.html");
	
	if (openfile.is_open())
	{		
		std::string temp;

		while (!openfile.eof())
		{
			
		}
	}
	
	openfile.close();
	
}







int main(int arc, char* argv[])
{


	//Init
	WSADATA data;
	SOCKET listener;
	struct sockaddr_in server;

	server.sin_family = AF_INET;
	server.sin_addr.S_un.S_addr = INADDR_ANY; // <-- ?
	server.sin_port = htons(8080);
	memset(server.sin_zero, 0, sizeof(server.sin_zero));

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
	std::cout << "Creating listener socket... ";
	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		std::cout << "Failed! Error: " << WSAGetLastError() << std::endl;
	}
	std::cout << "done!" << std::endl;


	std::cout << "Binding listener socket... ";
	if (bind(listener, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		std::cout << "Failed! Error: " << WSAGetLastError() << std::endl;
	}
	std::cout << "done!" << std::endl;


	//--------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------


	//Listening
	std::cout << "Listening...";
	if (listen(listener, SOMAXCONN) != 0)
	{
		std::cout << "Error: " << WSAGetLastError();
		closesocket(listener);
		WSACleanup();
		return 1;
	}
	std::cout << std::endl;



	WSAEVENT wsa_event = WSACreateEvent();
	WSAEventSelect(listener, wsa_event, FD_ACCEPT);



	while (true)
	{
		SOCKET new_socket;
		struct sockaddr_in remote;
		int size = sizeof(struct sockaddr_in);

		//char* message;

		if (WSAWaitForMultipleEvents(1, &wsa_event, TRUE, 10, FALSE) == WSA_WAIT_EVENT_0)
		{
			new_socket = accept(listener, (sockaddr*)&remote, &size);

			if (new_socket != INVALID_SOCKET)
			{
				std::cout << "New connection accepted!" << std::endl;

				static char buffer[4096] = { 0 };

				int bytes = recv(new_socket, buffer, sizeof(buffer)-1, 0);

				if (bytes < 0)
				{
					std::cout << "Could not recv()! Error: " << WSAGetLastError() << std::endl;
				}
				else if (bytes == 0)
				{
					//?
				}
				else
				{
					buffer[bytes] = '\0';
					std::string string_buffer = buffer;

					if (string_buffer.find("GET") != std::string::npos)
					{
						int getSTART = string_buffer.find("GET ") + sizeof("GET ") - 1;
						int getEND = string_buffer.find(" HTTP/");
						std::string getSTRING = string_buffer.substr(getSTART, getEND - getSTART);

						//defaulting index
						if (getSTRING == "/" || getSTRING == "/index")
							getSTRING = "/index.html";

						//Handle invalid get requests here

						//Check accepted types here

						//Load index.html

						//Send index.html
					}
				}
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



	//Reply
	message = "Hello client! Your connection has been recieved. It is now aborted, goodbye! \n";
	send(new_socket, message, strlen(message), 0);
*/