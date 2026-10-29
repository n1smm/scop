#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GLFW/glfw3.h>

#include "common_values.h"
#include "std_libs.h"
#include "mymath/mymath.hpp"


class Camera {
public:
    Camera(mymath::vec3	startPosition,
		   mymath::vec3	startUp,
		   GLfloat		startYaw,
		   GLfloat		startPitch,
		   GLfloat		startMoveSpeed,
		   GLfloat		startTurnSpeed);

    Camera(const Camera& other)				= delete;
    Camera& operator=(const Camera& other)	= delete;
    Camera(Camera&& other) 					noexcept = default;
    Camera& operator=(Camera&& other)		noexcept = default;

	void			keyControl(bool *keys, GLfloat deltaTime);
	void			mouseControl(bool *mouse_event, GLfloat xChange, GLfloat yChange);
	mymath::mat4	calculateViewMatrix();

	mymath::vec3	getCameraPosition();
	mymath::vec3	getCameraDirection();
	GLfloat			getYawOut();
	GLfloat			getPitchOut();

    ~Camera();

private:
	mymath::vec3	_position;
	mymath::vec3	_front;
	mymath::vec3	_up;
	mymath::vec3	_right;
	mymath::vec3	_world_up;
	GLfloat			_yaw;
	GLfloat			_pitch;
	GLfloat			_yaw_out;
	GLfloat			_pitch_out;
	GLfloat			_move_speed;
	GLfloat			_turn_speed;

	enum MouseEvent {
		MOUSE_FIRST_MOVED,
		LEFT_BUTTON,
		RIGHT_BUTTON,
		MIDDLE_BUTTON
	};

	void			_update();
};

#endif
