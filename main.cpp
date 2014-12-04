//main.cpp

#include "vld.h"
#include "HTTPOperations.hpp"


/*
	TO DO:

*	Chrome can load the page without problems. Firefox cannot. 
	Internet explorer is fucking stupid! I cannot even enter the ip address and port number directly, it searches the address and port on Bing instead... -_-
	-----

*	The header sent with the HTML page should not have a hard coded lenght in its description
	-----

*	WSACleanup is in the destructor, should I put it in a separate method instead? (any reason to do so?)
	-----

*	HTML snedpage and refuse memory leak (solved?)
	-----

*	Check if socket/client is already in list?
	-----

*	The while-loop that checks connections etc needs to be dependent on something insted of being a while(true)-loop.
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