#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include "common_values.h"
#include "utils.h"

class Shader {
public:
    Shader(const char *vertexCodePath, const char *fragmentCodePath);

    Shader(const Shader& other)				= delete;
    Shader& operator=(const Shader& other)	= delete;
    Shader(Shader&& other)					noexcept;
    Shader& operator=(Shader&& other)		noexcept;

	GLint	getUniformModelLocation();
	GLint	getUniformProjectionLocation();
	GLint	getUniformSurfaceSwitchLocation();
	GLint	getUniformViewLocation();
	GLint	getUniformInColorLocation();

	void	useShader();
	void	clearshader();
	void	setSurfaceState(SurfaceState newState);


    ~Shader();

private:
	GLuint				_shader_id;
	unsigned int		_shader_status;
	SurfaceState		_surface_state;
	GLint				_uniform_model, _uniform_projection, _uniform_view;
	GLint				_uniform_surface_switch, _uniform_in_color;
	std::vector<GLint>	_uniform_values;

	enum ShaderCodes {
		MY_SHADER_SUCCESS,
		MY_SHADER_FAIL,
		MY_SHADER_IN_USE,
		MY_SHADER_IDLE,
		MY_SHADER_CLEARED,
		MY_SHADER_EMPTY
	};

	void	_compileShader(const char *vertexCode, const char *fragmentCode);
	void	_addShader(GLuint shaderId, const char *shaderCode, GLenum shaderType);
};

#endif
