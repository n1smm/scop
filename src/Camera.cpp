#include "Camera.hpp"

Camera::Camera()
{
}

Camera::Camera(const Camera& other)
{
    (void)other;
}

Camera& Camera::operator=(const Camera& other)
{
    if (this != &other) {
    }
    return *this;
}

Camera::~Camera()
{
}
