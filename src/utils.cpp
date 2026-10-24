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

void	returnTmpMesh(std::vector<Mesh> *meshList)
{
	unsigned int	indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat			vertices[] = {
	//   x     y       z   		 u		v 	norm x     y     z 
		-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	Mesh	obj1;
	obj1.createMesh(vertices, indices, 32, 12);
	meshList->push_back(std::move(obj1));
}
