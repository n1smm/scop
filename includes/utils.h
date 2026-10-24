#ifndef UTILS_H
#define UTILS_H

#include "std_libs.h"
#include "Mesh.hpp"

#include <GL/gl.h>

std::string	readFromfile(const char* filePath);
void		returnTmpMesh(std::vector<Mesh> *meshList);

#endif
