#include "Shader.hpp"

Shader::Shader(const char *vertexCodePath, const char *fragmentCodePath)
{
	_shader_id = 0;
	_shader_status = MY_SHADER_EMPTY;
	_surface_state = SurfaceState::TEXTURE;

	_uniform_model = 0;
	_uniform_projection = 0;
	_uniform_view = 0;
	_uniform_surface_switch = 0;
	_uniform_in_color = 0;


	std::string vertexContent = readFromfile(vertexCodePath);
	std::string	fragmentContent = readFromfile(fragmentCodePath);
	if (vertexContent == "FAIL" || fragmentContent == "FAIL")
		throw	std::runtime_error("Shader: could not read shader source files");

	_compileShader(vertexContent.c_str(), fragmentContent.c_str());

	if (_shader_status == MY_SHADER_FAIL)
	{
		clearshader();
		throw std::runtime_error("Shader: compile/linking failed");
	}
}

Shader::Shader(Shader&& other) noexcept
	: _shader_id(std::exchange(other._shader_id, 0))
	, _shader_status(std::exchange(other._shader_status, MY_SHADER_EMPTY))
	, _surface_state(std::exchange(other._surface_state, SurfaceState::TEXTURE))
	, _uniform_model(std::exchange(other._uniform_model, 0))
	, _uniform_projection(std::exchange(other._uniform_projection, 0))
	, _uniform_view(std::exchange(other._uniform_view, 0))
	, _uniform_surface_switch(std::exchange(other._uniform_surface_switch, 1))
	, _uniform_in_color(std::exchange(other._uniform_in_color, 0))
	, _uniform_values(std::move(other._uniform_values))
{}
	

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this == &other)
		return *this;

	clearshader();

	_shader_id				= std::exchange(other._shader_id, 0);
	_shader_status			= std::exchange(other._shader_status, MY_SHADER_EMPTY);
	_surface_state			= std::exchange(other._surface_state, SurfaceState::TEXTURE);
	_uniform_model 			= std::exchange(other._uniform_model, 0);
	_uniform_projection		= std::exchange(other._uniform_projection, 0);
	_uniform_view			= std::exchange(other._uniform_view, 0);
	_uniform_surface_switch	= std::exchange(other._uniform_surface_switch, 1);
	_uniform_in_color		= std::exchange(other._uniform_in_color, 0);
	_uniform_values			= std::move(other._uniform_values);

	return *this;
}

void	Shader::useShader()
{
	if (_shader_id != 0 && _shader_status != MY_SHADER_FAIL)
	{
		glUseProgram(_shader_id);
		_shader_status = MY_SHADER_IN_USE;
	}

}

void	Shader::clearshader()
{
	if (_shader_id != 0)
	{
		glDeleteProgram(_shader_id);
		_shader_id = 0;
	}
	_shader_status = MY_SHADER_CLEARED;

	_uniform_model = 0;
	_uniform_projection = 0;
	_uniform_view = 0;
	_uniform_surface_switch = 0;
	_uniform_in_color = 0;
}

void	Shader::setSurfaceState(SurfaceState newState) { _surface_state = newState; }


/* getters */
GLint	Shader::getUniformModelLocation() { return _uniform_model; }
GLint	Shader::getUniformProjectionLocation() { return _uniform_projection; }
GLint	Shader::getUniformViewLocation() { return _uniform_view; }
GLint	Shader::getUniformSurfaceSwitchLocation() { return _uniform_surface_switch; }
GLint	Shader::getUniformInColorLocation() { return _uniform_in_color; }


/* private methods */
void	Shader::_compileShader(const char *vertexCode, const char *fragmentCode)
{
	_shader_id = glCreateProgram();

	if (!_shader_id)
	{
		std::cerr << "failed to initialize shader program";
		_shader_status = MY_SHADER_FAIL;
		return ;
	}

	_addShader(_shader_id, vertexCode, GL_VERTEX_SHADER);
	if (_shader_status == MY_SHADER_FAIL)
		return ;
	_addShader(_shader_id, fragmentCode, GL_FRAGMENT_SHADER);
	if (_shader_status == MY_SHADER_FAIL)
		return ;

	GLint	result = 0;
	GLchar	log[1024] = { 0 };

	glLinkProgram(_shader_id);
	glGetProgramiv(_shader_id, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(_shader_id, sizeof(log), NULL, log);
		std::cerr << "linking error: " << log << "\n";
		_shader_status = MY_SHADER_FAIL;
		return ;
	}

	glValidateProgram(_shader_id);
	glGetProgramiv(_shader_id, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(_shader_id, sizeof(log), NULL, log);
		std::cerr << "validation error: " << log << "\n";
		_shader_status = MY_SHADER_FAIL;
		return ;
	}

	_uniform_model = glGetUniformLocation(_shader_id, "model");
	_uniform_values.push_back(_uniform_model);
	_uniform_projection = glGetUniformLocation(_shader_id, "projection");
	_uniform_values.push_back(_uniform_projection);
	_uniform_view = glGetUniformLocation(_shader_id, "view");
	_uniform_values.push_back(_uniform_view);
	_uniform_surface_switch = glGetUniformLocation(_shader_id, "surfaceSwitch");
	_uniform_values.push_back(_uniform_surface_switch);
	_uniform_in_color = glGetUniformLocation(_shader_id, "inColor");

	for(size_t i = 0; i < _uniform_values.size(); i++)
	{
		if (_uniform_values[i] < 0)
		{
			std::cerr << "unable to get uniform value at _uniform_values[i] = " << i << "\n";
			_shader_status = MY_SHADER_FAIL;
			return ;
		}
	}
	_shader_status = MY_SHADER_SUCCESS;

}

void	Shader::_addShader(GLuint shaderId, const char *shaderCode, GLenum shaderType)
{
	GLuint	shader = glCreateShader(shaderType);
	if (!shader)
   {
       std::cerr << "failed to create shader object\n";
       _shader_status = MY_SHADER_FAIL;
       return ;
   }

	const	GLchar	*code[1];
	code[0] = shaderCode;

	GLint code_length[1];
	code_length[0] = strlen(shaderCode);

	glShaderSource(shader, 1, code, code_length);
	glCompileShader(shader);

	GLint	result = 0;
	GLchar	log[1024] = { 0 };

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(shader, sizeof(log), NULL, log);
		std::cerr << "error compiling" << shaderType << ": " << log << "\n";
		glDeleteShader(shader);
		_shader_status = MY_SHADER_FAIL;
		return ;
	}

	glAttachShader(shaderId, shader);
	glDeleteShader(shader);
}




Shader::~Shader()
{
	clearshader();
}
