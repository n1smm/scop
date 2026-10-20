#include "Mesh.hpp"

Mesh::Mesh()
{
}

Mesh::Mesh(const Mesh& other)
{
    (void)other;
}

Mesh& Mesh::operator=(const Mesh& other)
{
    if (this != &other) {
    }
    return *this;
}

Mesh::~Mesh()
{
}
