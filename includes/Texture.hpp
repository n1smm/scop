#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include	<GL/glew.h>
#include	"std_libs.h"
#include	"common_values.h"
#include	"utils.h"

class Texture {
public:
    Texture();
	explicit Texture(const char *filePath);
    Texture(const Texture& other)				= delete;
    Texture& operator=(const Texture& other)	= delete;
	Texture(Texture &&other)					noexcept;
	Texture	&operator=(Texture &&other)			noexcept;
	explicit operator bool()					const;

	void	useTexture(GLuint unit) const;
	void	clearTexture();

	//getter
	bool	isValid()				const;
	bool	isDefault()				const;

    ~Texture();

private:
	GLuint				_texture_id;
	int					_width, _height, _bit_depth;
	unsigned	char	*_pixels;
	const		char	*_file_path;
	bool				_is_default;

	bool			_loadFile(const char *filePath);
	bool			_loadTexture();
	bool			_freeFileContent();
};

#endif
