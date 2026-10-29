#ifdef STB_LOADER
# define STB_IMAGE_IMPLEMENTATION
# include <stb_image.h>
#endif

#include "Texture.hpp"

#ifndef STB_LOADER
static	uint32_t read_u32(const unsigned char *c)
{
	return c[0] | (c[1] << 8) | (c[2] << 16) | (c[3] << 24);
}

static uint16_t read_u16(const unsigned char *c)
{
	return (uint16_t)(c[0] | (c[1] << 8));
}

static int32_t read_i32(const unsigned char *c)
{
	int32_t value;
	memcpy(&value, c, sizeof(value));   // preserves the sign bit
	return value;
}

static void	bmpToRgba(const unsigned char *src, int bytePerPixel, unsigned char *dst)
{
	dst[0] = src[2];
	dst[1] = src[1];
	dst[2] = src[0];
	dst[3] = (bytePerPixel == 4) ? src[3] : 255;
}

static bool	loadBmpFile(const char *filePath, int *width, int *height,
						int *channels, unsigned char **pixels)
{
	std::ifstream file(filePath, std::ios::binary);
	if (!file)
	{
		std::cerr << "failed to open texture; path: " << filePath << std::endl;
		return false;
	}

	unsigned char header[14];
	file.read(reinterpret_cast<char *>(header), sizeof(header));
	if (!file || header[0] != 'B' || header[1] != 'M')
	{
		std::cerr << "unsupported, BMP only: " << filePath << std::endl;
		return false;
	}
	const uint32_t pxl_offset = read_u32(header + 10);

	unsigned char dib_size_read[4];
	file.read(reinterpret_cast<char *>(dib_size_read), sizeof(dib_size_read));
	if (!file)
		return false;

	const uint32_t dib_size = read_u32(dib_size_read);
	if (dib_size < 40)
	{
		std::cerr << "unsupported, BMP header size: " << filePath << std::endl;
		return false;
	}

	std::vector<unsigned char> dib(dib_size);
	memcpy(dib.data(), dib_size_read, sizeof(dib_size_read));
	file.read(reinterpret_cast<char *>(dib.data() + 4), dib_size - 4);
	if (!file)
		return false;

	int width_ 			= read_i32(dib.data() +4);
	int height_ 		= read_i32(dib.data() +8);
	uint16_t bit_p_px 	= read_u16(dib.data() + 14);

	if (bit_p_px != 24 && bit_p_px != 32)
	{
		std::cerr << "unsupported BMP bit depth: " << bit_p_px
			<< " (only 24 and 32)" << std::endl;
		return false;
	}

	bool topDown = (height_ < 0);

	const int byte_p_px = bit_p_px / 8;
	const int step = ((width_ * byte_p_px + 3) /4) * 4;

	unsigned char *out = (unsigned char *)malloc((size_t)width_ * height_ * 4);
	if (!out)
		return false;

	std::vector<unsigned char> row(step);

	file.seekg(pxl_offset, std::ios::beg);
	if (!file)
	{
		free(out);
		return false;
	}

	for (int y = 0; y < height_; y++)
	{
		file.read(reinterpret_cast<char *>(row.data()), step);
		if (!file)
		{
			std::cerr << "BMP ended unexpectedly at row " << y << std::endl;
			free(out);
			return false;
		}

		const int dstY = topDown ? y : (height_ - 1 - y);
		unsigned char *dstRow = out + (size_t)dstY * width_ * 4;

		for (int x = 0; x < width_; x++)
		{
			const unsigned char *src = row.data() + (size_t)x * byte_p_px;
			unsigned char       *dst = dstRow + (size_t)x * 4;
			bmpToRgba(src, byte_p_px, dst);
		}
	}

	*width    = width_;
	*height   = height_;
	*channels = 4;
	*pixels   = out;
	return true;
}
#endif

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

Texture::operator bool() const { return isValid(); }



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

//getter
bool	Texture::isValid() const { return _texture_id != 0; }


/* private methods */

bool			Texture::_loadFile(const char *filePath)
{
#ifdef STB_LOADER
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
#else
	unsigned char *tex_data = nullptr;

	if (!loadBmpFile(filePath, &_width, &_height, &_bit_depth, &tex_data))
	{
		_width = 0; _height = 0; _bit_depth = 0;
		if (_pixels)
		{
			free(_pixels);
			_pixels = nullptr;
		}
		return false;
	}

	if (_pixels)
		free(_pixels);
	_pixels = tex_data;

	return true;
#endif
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
		
		while (glGetError() != GL_NO_ERROR)
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

#ifdef STB_LOADER
	stbi_image_free(_pixels);
#else
	free(_pixels);
#endif
	_pixels = nullptr;
	return true;
}


Texture::~Texture()
{
	clearTexture();
}
