//HTTPOperations.cpp

#include "HTTPOperations.hpp"

#include <time.h>
#include <string>
#include <fstream>
#include <iostream>

#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// ***___ Constructor & Destructor ___***
HTTPOperations::HTTPOperations()
{}
HTTPOperations::~HTTPOperations()
{
	WSACleanup();
}


// ***___ void methods ___***
void HTTPOperations::Run()
{
	fd_set socketsList_fd;
	struct timeval timeInterval;

	timeInterval.tv_sec = 0;
	timeInterval.tv_usec = 10;

	while (true)
	{
		socketsList_fd.fd_count = 1;
		socketsList_fd.fd_array[0] = m_listenerSocket;

		//std::cout << "Waiting for activity. Timeout is " << timeInterval.tv_usec << std::endl;
		int activity = select(1, &socketsList_fd, 0, 0, &timeInterval);

		if (activity > 0)
		{
			AcceptNewConnection();
		}
		else if (activity == SOCKET_ERROR)
		{
			std::cout << "Select call returned socket error: " << WSAGetLastError() << std::endl;
		}

		HandleClients();
		Timeout();
		Remove();
	}
}

void HTTPOperations::AcceptNewConnection()
{
	SOCKET new_socket;
	struct sockaddr_in remote_sockaddr;
	int size = sizeof(struct sockaddr_in);

	std::cout << "Accepting new connection...";
	new_socket = accept(m_listenerSocket, (sockaddr*)&remote_sockaddr, &size);

	if (new_socket < 0)
	{
		std::cout << "failed! Socket error: " << WSAGetLastError() << std::endl;
	}
	else
	{
		std::cout << "accepted!" << std::endl;

		//check if already in list?

		AddClient(new_socket, remote_sockaddr);
	}
}
void HTTPOperations::AddClient(const SOCKET &sock, const struct sockaddr_in &addr)
{
	unsigned int timestamp = timeGetTime();

	std::cout << "Adding new client...";
	m_clients.push_back(CLIENT(sock, addr, timestamp, true));
	std::cout << "done. Client count: " << m_clients.size() << std::endl;;
}

void HTTPOperations::HandleClients()
{
	for (unsigned int i = 0; i < m_clients.size(); i++)
	{
		if (ClientHasData(m_clients[i]))
		{
			std::cout << "Client has data!" << std::endl;

			m_clients[i].m_timeStamp = timeGetTime();
			
			HandleClientRequest(m_clients[i]);
		}
	}
}
void HTTPOperations::HandleClientRequest(CLIENT &client)
{
	static char buffer[4096] = { 0 };
	int bytes = recv(client.m_socket, buffer, sizeof(buffer)-1, 0);

	if (bytes < 0)
	{
		std::cout << "Call on recv failed! Error: " << WSAGetLastError() << std::endl;
	}		
	else if (bytes == 0)
	{
		std::cout << "What?" << std::endl;
	}		
	else
	{
		std::cout << "Check request: ";

		buffer[bytes] = '\0';
		std::string buffer_as_string = buffer;

		//check if header contains keep-alive
		HeaderKeepAliveCheck(buffer_as_string, client);

		if (RequestValidation(buffer_as_string))
		{
			std::cout << "Request validated." << std::endl;

			if (FindGETRequest(buffer_as_string))
			{
				std::cout << "GET request recieved. Sending HTML page." << std::endl;
				HTMLSendPage(client.m_socket);
			}
			else
			{
				std::cout << "No GET request found!" << std::endl;
				HTMLRefuse(client.m_socket);
			}
		}
		else
		{
			std::cout << "Invalid request! Refusing input!" << std::endl;
			HTMLRefuse(client.m_socket);
			client.m_keepAlive = false;
		}
	}
}
void HTTPOperations::HeaderKeepAliveCheck(std::string &buffer_as_string, CLIENT &client)
{
	if (buffer_as_string.find("Connection: close") != std::string::npos)
	{
		client.m_keepAlive = false;
	}
	else if (buffer_as_string.find("Connection: keep-alive") != std::string::npos)
	{
		client.m_keepAlive = true;
	}		
}

// Check all clients to see if any have timed out. If they have, Keep-Alive is changed to false.
// Should be called just before Remove().
void HTTPOperations::Timeout()
{
	unsigned int now = timeGetTime();

	for (unsigned int i = 0; i < m_clients.size(); i++)
	{
		if (now - m_clients[i].m_timeStamp > 300000)
		{
			std::cout << "Client timed out." << std::endl;
			m_clients[i].m_keepAlive = false;
		}
	}
}

// Check all clients to see if their Keep-Alive is false. If it is, close the socket and remove the client.
// Should be called just after Timeout().
void HTTPOperations::Remove()
{
	auto iterator = m_clients.begin();

	while (iterator != m_clients.end())
	{
		if (!(*iterator).m_keepAlive)
		{
			//close socket
			closesocket((*iterator).m_socket);
			//remove client
			iterator = m_clients.erase(iterator);

			std::cout << "Client disconnected. Client count: " << m_clients.size() << std::endl;
		}
		else
		{
			++iterator;
		}
	}
}

void HTTPOperations::HTMLSendPage(const SOCKET &sock)
{
	std::string input = "";

	std::ifstream openfile;
	openfile.open("../WebPage/index.html");

	if (openfile.is_open())
	{
		std::string temp;

		while (!openfile.eof())
		{
			getline(openfile, temp);

			temp += "\r\n";
			input += temp;
		}
	}
	openfile.close();

	//header



	std::string header = "HTTP/1.1 200 OK\nContent-Lenght: 5000\n\n";
	//message
	char* message = new char[input.length() + header.length()];
	std::string headerAndMessage = header + input;
	message = const_cast<char*>(headerAndMessage.c_str());
	//send
	send(sock, message, strlen(message), 0);

	//delete message;
	//message = nullptr;
}
void HTTPOperations::HTMLRefuse(SOCKET &sock)
{
	std::string header = "HTTP/1.1 403 Forbidden\nContent-Lenght: 0\n\n";

	char* message = new char[header.length()];

	message = const_cast<char*>(header.c_str());

	send(sock, message, strlen(message), 0);

	//delete message;
	//message = nullptr;
}


// ***___ boolean methods ___***
bool HTTPOperations::Startup()
{
	if (Initialize())
	{
		if (CreateListenerSocket())
		{
			if (BindListenerSocket())
			{
				if (Listen())
				{
					return true;
				}
			}
		}
	}

	return false;
}

	//Startup's booleans
bool HTTPOperations::Initialize()
{
	m_serverSockaddr.sin_family = AF_INET;
	m_serverSockaddr.sin_addr.S_un.S_addr = INADDR_ANY; 
	m_serverSockaddr.sin_port = htons(8080);
	memset(m_serverSockaddr.sin_zero, 0, sizeof(m_serverSockaddr.sin_zero));

	std::cout << "Initializing winsock... ";
	if (WSAStartup(MAKEWORD(2, 1), &m_wsaData) != 0)
	{
		std::cout << "Failed! Error: " << WSAGetLastError() << std::endl;
		return false;
	}

	std::cout << "done!" << std::endl;
	return true;
}
bool HTTPOperations::CreateListenerSocket()
{
	std::cout << "Creating listener socket... ";
	if ((m_listenerSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		std::cout << "Failed! Error: " << WSAGetLastError() << std::endl;
		return false;
	}

	std::cout << "done!" << std::endl;
	return true;	
}
bool HTTPOperations::BindListenerSocket()
{
	std::cout << "Binding listener socket... ";
	if (bind(m_listenerSocket, (struct sockaddr*)&m_serverSockaddr, sizeof(m_serverSockaddr)) == SOCKET_ERROR)
	{
		std::cout << "Failed! Error: " << WSAGetLastError() << std::endl;
	}

	std::cout << "done!" << std::endl;
	return true;
}
bool HTTPOperations::Listen()
{
	if (listen(m_listenerSocket, SOMAXCONN) != 0)
	{
		std::cout << "Listen error: " << WSAGetLastError();
		return false;
	}
	std::cout << "Listening..." << std::endl;
	return true;
}

	//Datachecks

//returns true if client has data
bool HTTPOperations::ClientHasData(CLIENT &client)
{
	unsigned long int lenght = 0;
	ioctlsocket(client.m_socket, FIONREAD, &lenght);
	return lenght > 0;
}

//returns true if request is valid (does not contain "/..")
bool HTTPOperations::RequestValidation(std::string &buffer_as_string)
{
	if (buffer_as_string.find("/..") != std::string::npos)
	{
		return false;
	}

	return true;
}

//returns true if buffer contains GET request
bool HTTPOperations::FindGETRequest(std::string &buffer_as_string)
{
	if (buffer_as_string.find("GET") != std::string::npos)
	{
		return true;
	}
	
	return false;
}