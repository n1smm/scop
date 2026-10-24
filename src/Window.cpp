#include "Window.hpp"

Window::Window()
{
	_width = 0; _height = 0; _buffer_width = 0; _buffer_height = 0;
	_lastX = 0; _lastY = 0; _Xchange = 0; _Ychange = 0;
	_is_initialized = false;
	_mainWindow = nullptr;

	for(size_t i = 0; i < 1024; i++)
		_keys[i] = false;
	for(size_t i = 0; i < 10; i++)
		_mouse_events[i] = false;
	
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	_width = windowWidth; _height = windowHeight; _buffer_width = 0; _buffer_height = 0;
	_lastX = 0; _lastY = 0; _Xchange = 0; _Ychange = 0;
	_is_initialized = false;
	_mainWindow = nullptr;

	for(size_t i = 0; i < 1024; i++)
		_keys[i] = 0;
	for(size_t i = 0; i < 10; i++)
		_mouse_events[i] = 0;
}

Window::Window(const Window& other)
{
    (void)other;
}

Window& Window::operator=(const Window& other)
{
    if (this != &other) {
    }
    return *this;
}

int	Window::initialize()
{
	if (!glfwInit())
	{
		std::cerr << "glfw init failed \n";
		glfwTerminate();
		return 1;
	}
	_is_initialized = true;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	_mainWindow = glfwCreateWindow(_width, _height, "42 scop project", NULL, NULL);
	if (!_mainWindow)
	{
		std::cerr << "window not created\n";
		glfwTerminate();
		return 1;
	}

	glfwGetFramebufferSize(_mainWindow, &_buffer_width, &_buffer_height);
	glfwMakeContextCurrent(_mainWindow);

	_createCallBacks();
	glfwSetInputMode(_mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "glew initialization failed\n";
		glfwDestroyWindow(_mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, _buffer_width, _buffer_height);
	glfwSetWindowUserPointer(_mainWindow, this);

	return 0;
}

void	Window::swapBuffers()
{
	glfwSwapBuffers(_mainWindow);
}

void	Window::resetMouseDelta()
{
	_Xchange = 0.0f;
	_Ychange = 0.0f;
}


/* **** getters **** */
bool	Window::getWindowShouldClose()
{
	return glfwWindowShouldClose(_mainWindow);
}

GLfloat	Window::getBufferWidth()
{
	return _buffer_width;
}

GLfloat	Window::getBufferHeight()
{
	return _buffer_height;
}

GLfloat	Window::getXchange()
{
	return _Xchange;
}

GLfloat	Window::getYchange()
{
	return _Ychange;
}

bool	*Window::getKeys()
{
	return _keys;
}

bool	*Window::getMouseEvents()
{
	return _mouse_events;
}


/* **** private methods **** */

void	Window::_createCallBacks()
{
	glfwSetKeyCallback(_mainWindow, _handleKeys);
	glfwSetCursorPosCallback(_mainWindow ,_handleMouseMove);
	glfwSetMouseButtonCallback(_mainWindow, _handleMouseBtn);
}

void	Window::_handleKeys(GLFWwindow *win, int key, int code, int action, int mode)
{
	Window	*currWin = static_cast<Window*>(glfwGetWindowUserPointer(win));
	if (!currWin || !currWin->_mainWindow)
	{
		std::cerr << "window passed to _handleKeys is invalid\n";
		return ;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(currWin->_mainWindow, GL_TRUE);
		std::cout << "pressed escape\n";
	}

	if (key > 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			currWin->_keys[key] = true;
		else if (action == GLFW_RELEASE)
			currWin->_keys[key] = false;
	}
	(void)code;
	(void)mode;
}

void	Window::_handleMouseMove(GLFWwindow *win, double xPos, double yPos)
{
	Window	*currWin = static_cast<Window*>(glfwGetWindowUserPointer(win));
	if (!currWin || !currWin->_mainWindow)
	{
		std::cerr << "window passed to _handleKeys is invalid\n";
		return ;
	}
	if (!currWin->_mouse_events[MOUSE_FIRST_MOVED])
	{
		currWin->_mouse_events[MOUSE_FIRST_MOVED] = true;
		currWin->_lastX = xPos;
		currWin->_lastY = yPos;
		std::cout << "mouse first moved!\n";
		return ;
	}

	currWin->_Xchange = currWin->_lastX - xPos;
	currWin->_Ychange = currWin->_lastY - yPos;
	currWin->_lastX = xPos;
	currWin->_lastY = yPos;
}

void	Window::_handleMouseBtn(GLFWwindow *win, int button, int action, int mods)
{
	Window	*currWin = static_cast<Window*>(glfwGetWindowUserPointer(win));
	if (!currWin || !currWin->_mainWindow)
	{
		std::cerr << "window passed to _handleKeys is invalid\n";
		return ;
	}
	if (action == GLFW_PRESS)
	{
		switch (button) {
			case GLFW_MOUSE_BUTTON_LEFT:
				currWin->_mouse_events[LEFT_BUTTON] = true;
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				currWin->_mouse_events[RIGHT_BUTTON] = true;
				break;
			case GLFW_MOUSE_BUTTON_MIDDLE:
				currWin->_mouse_events[MIDDLE_BUTTON] = true;
				break;
			default:
				break;
		}

	}
	else if (action == GLFW_RELEASE)
	{
		switch (button) {
			case GLFW_MOUSE_BUTTON_LEFT:
				currWin->_mouse_events[LEFT_BUTTON] = false;
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				currWin->_mouse_events[RIGHT_BUTTON] = false;
				break;
			case GLFW_MOUSE_BUTTON_MIDDLE:
				currWin->_mouse_events[MIDDLE_BUTTON] = false;
				break;
			default:
				break;
		}
	}
	(void)mods;
}

Window::~Window()
{
	if (_mainWindow)
		glfwDestroyWindow(_mainWindow);
	if (_is_initialized)
		glfwTerminate();

	_width = 0; _height = 0; _buffer_width = 0; _buffer_height = 0;
	_lastX = 0; _lastY = 0; _Xchange = 0; _Ychange = 0;
	_is_initialized = false;
	_mainWindow = nullptr;
}
