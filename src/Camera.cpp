#include "Camera.hpp"

Camera::Camera(mymath::vec3	startPosition,
	   mymath::vec3	startUp,
	   GLfloat		startYaw,
	   GLfloat		startPitch,
	   GLfloat		startMoveSpeed,
	   GLfloat		startTurnSpeed)
{
	_position	= startPosition;
	_world_up	= startUp;
	_yaw		= startYaw;
	_pitch		= startPitch;
	_move_speed	= startMoveSpeed;
	_turn_speed	= startTurnSpeed;

	_front		= mymath::vec3(0.0f, 0.0f, -1.0f);
	_update();

}

// Camera::Camera(Camera&& other) noexcept 
// 	: _position(std::exchange(other._position, mymath::vec3(0.0f, 0.0f, 0.0f)))
// 	, _world_up(std::exchange(other._world_up, mymath::vec3(0.0f, 1.0f, 0.0f)))
// {
// }

// Camera& Camera::operator=(Camera&& other) noexcept
// {
//     if (this != &other) {
//     }
//     return *this;
// }


void			Camera::keyControl(bool *keys, GLfloat deltaTime)
{
	GLfloat velocity = _move_speed * deltaTime;

	if (keys[GLFW_KEY_W])
		_position += _front * velocity;
	if (keys[GLFW_KEY_S])
		_position -= _front * velocity;
	if (keys[GLFW_KEY_A])
		_position -= _right * velocity;
	if (keys[GLFW_KEY_D])
		_position += _right * velocity;

}

void			Camera::mouseControl(bool *mouse_event, GLfloat xChange, GLfloat yChange)
{
	if (!mouse_event[LEFT_BUTTON])
		return ;

	xChange *= _turn_speed;
	yChange *= _turn_speed;

	_yaw -= xChange;
	_pitch += yChange;

	if (_pitch > 89.0f)
		_pitch = 89.0f;
	if (_pitch < -89.0f)
		_pitch = -89.0f;
	_update();
}

mymath::mat4	Camera::calculateViewMatrix()
{
	return mymath::lookAt(_position, _position + _front, _up);
}

/* getters */
mymath::vec3	Camera::getCameraPosition() { return _position; }

mymath::vec3	Camera::getCameraDirection() { return mymath::normalize(_front); }

/* private methods */

void	Camera::_update()
{
	_front.x	= cos(mymath::radians(_yaw)) * cos(mymath::radians(_pitch));
	_front.y	= sin(mymath::radians(_pitch));
	_front.z	= sin(mymath::radians(_yaw)) * cos(mymath::radians(_pitch));

	_right 		= mymath::normalize(mymath::cross(_front, _world_up));
	_up			= mymath::normalize(mymath::cross(_right, _front)); 	
}

Camera::~Camera()
{
}
