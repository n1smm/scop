#ifndef MY_MATH_H
#define MY_MATH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <utility>

namespace mymath {
	using mat4 = glm::mat4;
	using vec3 = glm::vec3;
	using vec4 = glm::vec4;

	inline	mat4	rotate(const mat4 &mat, float angle, const vec3 &axis) 
	{
		return glm::rotate(mat, angle, axis);
	}

	inline	mat4	translate(const mat4 &mat, const vec3 &vec) 
	{
		return glm::translate(mat, vec);
	}

	inline	mat4	scale(const mat4 &mat, const vec3 &vec) 
	{
		return glm::scale(mat, vec);
	}

	inline	mat4	perspective(float fovY, float aspect, float near, float far)
	{
		return glm::perspective(fovY, aspect, near, far);
	}

	inline	mat4	lookAt(const vec3 &eye, const vec3& center, const vec3 &up)
	{
		return glm::lookAt(eye, center, up);
	}

	inline	vec3	normalize(const vec3 &vec)
	{
		return glm::normalize(vec);
	}

	inline	vec3	cross(const vec3 &vecX, const vec3 &vecY)
	{
		return glm::cross(vecX, vecY);
	}

	inline	float	dot(const vec3 &vecX, const vec3 &vecY)
	{
		return glm::dot(vecX, vecY);
	}

	inline	float	radians(float degrees)
	{
		return glm::radians(degrees);
	}

	template	<typename T>
	inline	auto	value_ptr(T &&obj) -> decltype(glm::value_ptr(std::forward<T>(obj)))
	{
		return glm::value_ptr(std::forward<T>(obj));
	}
}

#endif
