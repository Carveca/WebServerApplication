//HTTPOperations.cpp

#include "HTTPOperations.hpp"

#include <string>
#include <fstream>

HTTPOperations::HTTPOperations()
{

}



void HTTPOperations::LoadHTTPFile()
{
	std::string input = "";

	std::ifstream openfile;
	openfile.open("../WebPage/index.html");

	if (openfile.is_open())
	{
		std::string temp;

		while (!openfile.eof())
		{
			openfile >> temp;
			temp += "\r\n";
			input += temp;
		}
	}
	openfile.close();


}