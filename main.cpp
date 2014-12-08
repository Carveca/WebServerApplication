//main.cpp

#include "vld.h"
#include "HTTPOperations.hpp"

int main(int arc, char* argv[])
{
	HTTPOperations server;

	if (server.Startup())
	{
		server.Run();
	}
	
	return 0;
}