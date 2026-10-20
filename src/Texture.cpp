#include "Texture.hpp"

Texture::Texture()
{
}

Texture::Texture(const Texture& other)
{
    (void)other;
}

Texture& Texture::operator=(const Texture& other)
{
    if (this != &other) {
    }
    return *this;
}

Texture::~Texture()
{
}
