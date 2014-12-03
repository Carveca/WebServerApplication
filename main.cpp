//main.cpp

#include "vld.h"

#include <time.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <WinSock2.h>

#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

struct CLIENT
{
	CLIENT(const SOCKET &sock, const sockaddr_in &addr, unsigned int &timestamp, const bool &keepalive) 
	{
		m_socket = sock;
		m_addr = addr;
		m_timeStamp = timestamp;
		m_keepAlive = keepalive;
	}

	SOCKET m_socket;
	struct sockaddr_in m_addr;
	unsigned int m_timeStamp;
	bool m_keepAlive;
};

void HTMLSendPage(const SOCKET sock)
{
	std::string input = "";

	std::ifstream openfile;
	openfile.open("../WebPage/index.html");

	if (openfile.is_open())
	{		
		std::string temp;

		while (!openfile.eof())
		{			
			//openfile >> temp;

			getline(openfile, temp);

			temp += "\r\n";
			input += temp;
		}
	}	
	openfile.close();
	
	//header
	std::string header = "HTTP/1.1 200 OK\nContent-Lenght: 5000\n\n";
	
	char* message = new char[input.length() + header.length()];

	std::string headerAndMessage = header + input;

	message = const_cast<char*>(headerAndMessage.c_str());

	send(sock, message, strlen(message), 0);

	delete message;
	message = nullptr;
}

void HTMLRefuse(SOCKET &sock)
{
	std::string header = "HTTP/1.1 403 Forbidden\nContent-Lenght: 0\n\n";

	char* message = new char[header.length()];

	message = const_cast<char*>(header.c_str());

	send(sock, message, strlen(message), 0);

	delete message;
	message = nullptr;
}



bool ClientHasData(CLIENT &client)
{
	unsigned long int lenght = 0;
	ioctlsocket(client.m_socket, FIONREAD, &lenght);
	return lenght > 0;
}



//Remove clients
//varje update : kolla alla clients i vector om timestamp �r �ver (ex 10 sec) eller keepalive = false, ta bort client
//ta bort client = st�ng socket, ta bort ur listan



//HANDLE INVALID GET REQUESTS:
//if "/.." finns i GET, skicka forbidden eller 404 eller n�tt




int main(int arc, char* argv[])
{
	std::vector<CLIENT> m_clients;

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

	float time = 0.0f;

	while (time < 10.0f) //make this dependent on something instead, time without connections?
	{
		time += 0.01f;

		SOCKET new_socket;
		struct sockaddr_in remote;
		int size = sizeof(struct sockaddr_in);

		new_socket = accept(listener, (sockaddr*)&remote, &size);

		if (new_socket != INVALID_SOCKET)
		{
			std::cout << "New connection accepted!" << std::endl;

			unsigned int time = timeGetTime();
			CLIENT new_client(new_socket, remote, time, true);
			m_clients.push_back(new_client);

			for (unsigned int i = 0; i < m_clients.size(); i++)
			{
				if (ClientHasData(m_clients[i]))
				{
					static char buffer[4096] = { 0 };
					int bytes = recv(m_clients[i].m_socket, buffer, sizeof(buffer) - 1, 0);

					if (bytes < 0)
					{
						std::cout << "Could not recv()! Error: " << WSAGetLastError() << std::endl;
					}
					else if (bytes == 0)
					{
						std::cout << "What?" << std::endl;
					}
					else
					{
						std::cout << "Get request" << std::endl;

						m_clients[i].m_timeStamp = timeGetTime();

						buffer[bytes] = '\0';
						std::string string_buffer = buffer;

						//handle invalid get
						if (string_buffer.find("/..") != std::string::npos)
						{
							HTMLRefuse(m_clients[i].m_socket);
							//remove client?
						}
						else
						{
							//check keepalive
							if (string_buffer.find("Connection: close") != std::string::npos)
								m_clients[i].m_keepAlive = false;


							//handle get requests
							if (string_buffer.find("GET") != std::string::npos)
							{
								int getSTART = string_buffer.find("GET ") + sizeof("GET ") - 1;
								int getEND = string_buffer.find(" HTTP/");
								std::string getSTRING = string_buffer.substr(getSTART, getEND - getSTART);

								HTMLSendPage(new_socket);
							}

							//check keepAlive

						}
					}
				}					
			}
		}
	}
	




	//Close
	//WSACloseEvent(wsa_event);
	closesocket(listener);
	WSACleanup();
	return 0;
}


//list of clients (otherwise socket goes out of scope);






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

/*
		OLD CODE 2:

		WSAEVENT wsa_event = WSACreateEvent();
		WSAEventSelect(listener, wsa_event, FD_ACCEPT);



		while (true)
		{
		SOCKET new_socket;
		struct sockaddr_in remote;
		int size = sizeof(struct sockaddr_in);

		if (WSAWaitForMultipleEvents(1, &wsa_event, TRUE, 10, FALSE) == WSA_WAIT_EVENT_0)
		{
		new_socket = accept(listener, (sockaddr*)&remote, &size);

		if (new_socket != INVALID_SOCKET)
		{
		std::cout << "New connection accepted!" << std::endl;

		static char buffer[4096] = { 0 };

		int bytes = recv(new_socket, buffer, sizeof(buffer)-1, 0);

		Sleep(1000);

		if (bytes < 0)
		{
		std::cout << "Could not recv()! Error: " << WSAGetLastError() << std::endl;
		}
		else if (bytes == 0)
		{
		std::cout << "What?" << std::endl;
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

		HTMLSendPage(new_socket);
		}
		}
		}
		}
		}



*/

/*
		OLD CODE 3:

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



*/