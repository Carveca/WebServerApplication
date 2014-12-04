//main.cpp

#include "vld.h"
#include "HTTPOperations.hpp"


/*
	TO DO:

*	Chrome can load the page without problems. Firefox cannot. 
	Internet explorer is fucking stupid! I cannot even enter the ip address and port number directly, it searches the address and port on Bing instead... -_-
	-----

*	The headersent with the HTML page should not have a hard coded lenght in its description
	-----

*	WSACleanup is in destructor, put it in a separate method? (any reason to?)
	-----

*	Do I need to delete the char* in HTMLsendpage and HTMLrefuse? Program acts wierd when I do so...
	-----

*	Check if socket/client is already in list?
	-----

*/

int main(int arc, char* argv[])
{
	HTTPOperations server;

	if (server.Startup())
	{
		server.Run();
	}


	return 0;
}