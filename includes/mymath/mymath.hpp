#ifndef MY_MATH_H
#define MY_MATH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// #include <GL/gl.h>
#include <utility>

#include "vec3.hpp"
#include "mat4.hpp"

namespace mymath {
	// using mat4 = glm::mat4;
	// using vec3 = glm::vec3;
	using vec4 = glm::vec4;


	mat4	rotate(const mat4 &mat, float angle, const vec3 &axis);

	mat4	translate(const mat4 &mat, const vec3 &vec);

	mat4	scale(const mat4& m, const vec3& factors);

	mat4	perspective(float fovy, float aspect, float zNear, float zFar);

	mat4	lookAt(const vec3 &eye, const vec3& center, const vec3 &up);

	vec3	normalize(const vec3 &vec);

	vec3	cross(const vec3 &vecX, const vec3 &vecY);

	float	dot(const vec3 &vecX, const vec3 &vecY);

	float	radians(float degrees);

	float		*value_ptr(mat4 &m);
	const float	*value_ptr(const mat4 &m);

	// template	<typename T>
	// inline	auto	value_ptr(T &&obj) -> decltype(glm::value_ptr(std::forward<T>(obj)))
	// {
	// 	return glm::value_ptr(std::forward<T>(obj));
	// }
}

#endif
