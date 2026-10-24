#include "utils.h"


std::string	readFromfile(const char *filePath)
{
	std::string 	content = "";
	std::ifstream	file_stream(filePath, std::ios::in);

	if (!file_stream)
	{
		std::cerr << "failed to read from file: " << filePath << std::endl;
		return "FAIL";
	}

	std::string line = "";
	while(!file_stream.eof())
	{
		std::getline(file_stream, line);
		content.append(line + "\n");
		line = "";
	}
	file_stream.close();

	if (content.empty())
	{
		std::cerr << "empty file: " << filePath << std::endl;
		return "FAIL";
	}
	return content;
}
