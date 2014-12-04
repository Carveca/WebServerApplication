//HTTPOperations.hpp

#pragma once

#include <WinSock2.h>
#include <vector>


class HTTPOperations
{
public:
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


public:
	HTTPOperations();
	~HTTPOperations();

	void Run();
	
	void AcceptNewConnection();
	void AddClient(const SOCKET &sock, const struct sockaddr_in &addr);
	
	void HandleClients();
	void HandleClientRequest(CLIENT &client);
	void HeaderKeepAliveCheck(std::string &buffer_as_string, CLIENT &client);

	void Timeout();
	void Remove();

	void HTMLSendPage(const SOCKET &sock);
	void HTMLRefuse(SOCKET &sock);

	bool Startup();

	bool Initialize();
	bool CreateListenerSocket();
	bool BindListenerSocket();
	bool Listen();

	bool ClientHasData(CLIENT &client);
	bool RequestValidation(std::string &buffer_as_string);
	bool FindGETRequest(std::string &buffer_as_string);

private:
	std::vector<CLIENT> m_clients;

	WSADATA m_wsaData;
	SOCKET m_listenerSocket;
	struct sockaddr_in m_serverSockaddr;
};