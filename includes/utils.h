#ifndef UTILS_H
#define UTILS_H

#include "std_libs.h"
#include "Mesh.hpp"

#include <GL/gl.h>


std::vector<std::string_view> split_line(std::string_view str);
std::string	readFromfile(const char* filePath);
void		returnTmpMesh(std::vector<Mesh> *meshList);

template <typename T>
bool	toNumber(std::string_view token, T &out)
{
	const char *first	= token.data();
	const char *last	= token.data() + token.size();

	const auto [end, errCode] = std::from_chars(first, last, out);
	return (errCode == std::errc() && end == last);
}

#endif
