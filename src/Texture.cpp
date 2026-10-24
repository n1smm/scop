#define STB_IMAGE_IMPLEMENTATION
#include	<stb_image.h>
#include "Texture.hpp"

Texture::Texture() : _texture_id(0)
						, _width(0), _height(0), _bit_depth(0)
						, _pixels(nullptr)
						, _file_path(DEFAULT_TEX_PATH)
{
	if (!_loadTexture())
	{
		clearTexture();
		std::cerr << "failed to load default texture" << std::endl;
	}
}

Texture::Texture(const char *filePath) : _texture_id(0)
												, _width(0), _height(0), _bit_depth(0)
												, _pixels(nullptr)
												, _file_path(DEFAULT_TEX_PATH)
{
	_file_path = filePath;
	bool success = _loadTexture();

	if (!success)
	{
		std::cerr << "switching to load texture from default path" << std::endl;
		_freeFileContent();
		_file_path = DEFAULT_TEX_PATH;
		success = _loadTexture();
	}

	if (!success)
	{
		std::cerr << "couldn't load any texture, skipping ... " << std::endl;
		clearTexture();
	}
}

Texture::Texture(Texture &&other)					noexcept
					: _texture_id(std::exchange(other._texture_id, 0))
					, _width(std::exchange(other._width, 0))
					, _height(std::exchange(other._height, 0))
					, _pixels(std::exchange(other._pixels, nullptr))
					, _file_path(std::exchange(other._file_path, nullptr))
{}

Texture	&Texture::operator=(Texture &&other)			noexcept
{
	if (this == &other)
		return *this;

	clearTexture();
	_texture_id		= std::exchange(other._texture_id, 0);
	_width			= std::exchange(other._width, 0);
	_height			= std::exchange(other._height, 0);
	_bit_depth		= std::exchange(other._bit_depth, 0);
	_pixels			= std::exchange(other._pixels, nullptr);
	_file_path		= std::exchange(other._file_path, nullptr);

	return *this;
}



/* public methods */

void	Texture::useTexture(GLuint unit) const
{
	if (_texture_id == 0)
		return;

	static const GLint maxUnits = []{
		GLint n = 0;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &n);
		return n;
	}();
	if (unit >= (GLuint)maxUnits)
		return ;

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, _texture_id);
}

void	Texture::clearTexture()
{
	if (_texture_id != 0)
		glDeleteTextures(1, &_texture_id);
	_freeFileContent();
	_texture_id = 0;
	_width = 0; _height = 0; _bit_depth = 0;
	_file_path = nullptr;
	_pixels = nullptr;

}


/* private methods */

bool			Texture::_loadFile(const char *filePath)
{
	// Force STBI_rgb_alpha so _pixels is always 4 bytes/pixel (RGBA),
	// whatever the file's native channel count. _bit_depth still receives
	// the FILE's native channel count (info only, not used for the upload).
	unsigned char *tex_data = stbi_load(filePath, &_width, &_height, &_bit_depth, STBI_rgb_alpha);

	if (!tex_data)
	{
		std::cerr << "failed to load texture; path: " << filePath <<
			(filePath ? stbi_failure_reason() : "null path") << std::endl;
		_width = 0; _height = 0; _bit_depth = 0;
		if (_pixels)
		{
			stbi_image_free(_pixels);
			_pixels = nullptr;
		}
		return false;
	}

	if (_pixels)
		stbi_image_free(_pixels);
	_pixels = tex_data;

	return true;
}

bool			Texture::_loadTexture()
{
	if (!_loadFile(_file_path))
		return false;

	glGenTextures(1, &_texture_id);
	if (_texture_id == 0)
	{
		std::cerr << "failed to generate texture object" << std::endl;
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, _texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		while (glGetError() != GL_NO_ERROR)	// drain stale errors
			;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixels);
		glGenerateMipmap(GL_TEXTURE_2D);

		GLenum	err = glGetError();
		if (err != GL_NO_ERROR)
		{
			std::cerr << "OpenGL error during texture upload: 0x"
					<< std::hex << err << std::dec << std::endl;
			glBindTexture(GL_TEXTURE_2D, 0);
			glDeleteTextures(1, &_texture_id);
			_texture_id = 0;
			_freeFileContent();
			return false;
		}

	glBindTexture(GL_TEXTURE_2D, 0);
	_freeFileContent();
	return true;
}

bool			Texture::_freeFileContent()
{
	if (!_pixels)
		return false;

	stbi_image_free(_pixels);
	_pixels = nullptr;
	return true;
}


Texture::~Texture()
{
	clearTexture();
}
