#ifndef COMMON_VALUES_H
#define COMMON_VALUES_H
#include "types.h"
#include <GL/gl.h>

constexpr		 	unsigned int WIDTH = 1366, HEIGHT = 768;

constexpr	const	char		*vShaderPath = "shaders/shader.vert";
constexpr	const	char		*fShaderPath = "shaders/shader.frag";
#ifdef STB_LOADER
	constexpr	const	char		*DEFAULT_TEX_PATH = "textures/default/plain.png";
#else
	constexpr	const	char		*DEFAULT_TEX_PATH = "textures/default/shrek.bmp";
#endif


enum SurfaceState {
	COLOR,
	TEXTURE,
	POINT,
	WIREFRAME,
	COUNT
};



#endif
