#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GL/glew.h>
#include "common_values.h"
#include "std_libs.h"
#include "GLFW/glfw3.h"

class Window {
public:
    Window();
    Window(GLint windowWidth, GLint windowHeight);
    Window(const Window& other);
    Window& operator=(const Window& other);

	int		initialize();
	void	swapBuffers();
	void	resetMouseDelta();
	//getters
	GLfloat	getBufferWidth();
	GLfloat	getBufferHeight();
	GLfloat	getXchange();
	GLfloat	getYchange();
	bool	*getKeys();
	bool	*getMouseEvents();

	bool	getWindowShouldClose();

    ~Window();

private:
	GLFWwindow		*_mainWindow;
	GLint			_width, _height, _buffer_width, _buffer_height;
	GLfloat			_lastX, _lastY, _Xchange, _Ychange;
	bool			_is_initialized;
	bool			_keys[1024];
	bool			_mouse_events[10];

	enum MouseEvent {
		MOUSE_FIRST_MOVED,
		LEFT_BUTTON,
		RIGHT_BUTTON,
		MIDDLE_BUTTON
	};

	void			_createCallBacks();
	static	void	_handleKeys(GLFWwindow *win, int key, int code, int action, int mode);
	static	void	_handleMouseMove(GLFWwindow *win, double xPos, double yPos);
	static	void	_handleMouseBtn(GLFWwindow *win, int button, int action, int mods);

};

#endif
