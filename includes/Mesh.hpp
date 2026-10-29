#ifndef MESH_HPP
#define MESH_HPP

#include <GL/glew.h>
#include "common_values.h"
#include "std_libs.h"


class Mesh {
public:
	enum		RenderMode {
		FILLED,
		WIREFRAME,
		POINTS
	};

    Mesh();
    Mesh(const Mesh &other)				= delete;
    Mesh &operator=(const Mesh &other)	= delete;
	Mesh(Mesh &&other) 					noexcept;
	Mesh &operator=(Mesh &&other)		noexcept;
	explicit operator bool() 			const;

	void	createMesh(GLfloat *vertices, unsigned int *indices, GLuint numVertices, GLuint numIndices);
	void	renderMesh(RenderMode mode = RenderMode::FILLED);
	void	clearMesh();

	//getter
	bool	isValid() const;

    ~Mesh();

private:
	GLuint		_VAO, _VBO, _IBO;
	GLsizei		_indexCount;

};

#endif
