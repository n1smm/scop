#ifndef MESH_HPP
#define MESH_HPP

#include <GL/glew.h>
#include "common_values.h"
#include "std_libs.h"

class Mesh {
public:
    Mesh();
    Mesh(const Mesh &other)				= delete;
    Mesh &operator=(const Mesh &other)	= delete;
	Mesh(Mesh &&other) 					noexcept;
	Mesh &operator=(Mesh &&other)		noexcept;

	void	createMesh(GLfloat *vertices, unsigned int *indices, GLuint numVertices, GLuint numIndices);
	void	renderMesh();
	void	clearMesh();

    ~Mesh();

private:
	GLuint		_VAO, _VBO, _IBO;
	GLsizei		_indexCount;
};

#endif
