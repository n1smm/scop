#include "Mesh.hpp"

Mesh::Mesh()
{
	_VAO = 0; _VBO = 0; _IBO = 0;
	_indexCount = 0;
}

Mesh::Mesh(Mesh &&other) noexcept
	: _VAO(std::exchange(other._VAO, 0))
	, _VBO(std::exchange(other._VBO, 0))
	, _IBO(std::exchange(other._IBO, 0))
	, _indexCount(std::exchange(other._indexCount, 0))
{
}

Mesh& Mesh::operator=(Mesh &&other)	noexcept
{
	if (this == &other)
		return *this;

	clearMesh();

    _VAO        = std::exchange(other._VAO, 0);
    _VBO        = std::exchange(other._VBO, 0);
    _IBO        = std::exchange(other._IBO, 0);
    _indexCount = std::exchange(other._indexCount, 0);
    return *this;

}

Mesh::operator bool() const { return isValid(); }


void	Mesh::createMesh(GLfloat *vertices, unsigned int *indices, GLuint numVertices, GLuint numIndices)
{
	clearMesh();

	_indexCount = numIndices;

	glGenVertexArrays(1, &_VAO);
	glBindVertexArray(_VAO);

		glGenBuffers(1, &_IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numIndices, indices, GL_STATIC_DRAW);

			glGenBuffers(1, &_VBO);
			glBindBuffer(GL_ARRAY_BUFFER, _VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numVertices, vertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0);
				glEnableVertexAttribArray(0);

				
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) *3));
				glEnableVertexAttribArray(1);

				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) *5));
				glEnableVertexAttribArray(2);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void	Mesh::renderMesh(RenderMode mode)
{
	if (!isValid())
		return ;

	glBindVertexArray(_VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);
			switch (mode)
			{
				case RenderMode::FILLED:
					glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);
					break;
				case RenderMode::POINTS:
					glDrawElements(GL_POINTS, _indexCount, GL_UNSIGNED_INT, 0);
					break;
				case RenderMode::WIREFRAME:
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					break;


			}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void	Mesh::clearMesh()
{
	_indexCount = 0;

	if (_IBO != 0)
	{
		glDeleteBuffers(1, &_IBO);
		_IBO = 0;
	}
	if (_VBO != 0)
	{
		glDeleteBuffers(1, &_VBO);
		_VBO = 0;
	}
	if (_VAO != 0)
	{
		glDeleteVertexArrays(1, &_VAO);
		_VAO = 0;
	}
}

//getter
bool	Mesh::isValid() const { return _VAO != 0 && _indexCount > 0; }



Mesh::~Mesh()
{
	clearMesh();
}
