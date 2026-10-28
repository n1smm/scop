#include "mymath/mymath.hpp"
#include "std_libs.h"

namespace mymath {


	float		*value_ptr(mat4 &m) { return &m[0][0]; }
	const float	*value_ptr(const mat4 &m) { return &m[0][0]; }

	float radians(float degrees) 
	{ return degrees * 0.01745329251994329576923690768489f; }

	vec3	normalize(const vec3 &vec)
	{
		float len = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
		if (len > 0.0f)
		{
			float inv = 1.0f / len;
			return {vec.x * inv, vec.y * inv, vec.z * inv};
		}
		return vec;
	}

	vec3	cross(const vec3 &vecX, const vec3 &vecY)
	{
		vec3 product;
		product.x = vecX.y * vecY.z - vecX.z * vecY.y;
		product.y = -(vecX.x * vecY.z - vecX.z * vecY.x);
		product.z = vecX.x * vecY.y - vecX.y * vecY.x;
		return product;
	}

	float	dot(const vec3 &vecX, const vec3 &vecY)
	{
		return  vecX.x * vecY.x + vecX.y * vecY.y + vecX.z * vecY.z;
	}

	mat4 	perspective(float fovy, float aspect, float zNear, float zFar)
	{
		float tanHalfFovy = std::tan(fovy / 2.0f);
		float focalLength = 1.0f / tanHalfFovy;
		float zScale      = -(zFar + zNear) / (zFar - zNear);
		float zOffset     = -(2.0f * zFar * zNear) / (zFar - zNear);

		mat4 result(0.0f);
		result[0][0] = focalLength / aspect;   // x
		result[1][1] = focalLength;            // y
		result[2][2] = zScale;                 // z
		result[2][3] = -1.0f;                  // w
		result[3][2] = zOffset;                // z
		return result;
	}

	mat4 	lookAt(const vec3& eye, const vec3& center, const vec3& worldUp)
	{
		vec3 forward  = normalize(center - eye);
		vec3 right    = normalize(cross(forward, worldUp));
		vec3 cameraUp = cross(right, forward);

		mat4 view(1.0f);

		// rotation part
		view[0][0] = right.x;     view[1][0] = right.y;     view[2][0] = right.z;
		view[0][1] = cameraUp.x;  view[1][1] = cameraUp.y;  view[2][1] = cameraUp.z;
		view[0][2] = -forward.x;  view[1][2] = -forward.y;  view[2][2] = -forward.z;

		// translation
		view[3][0] = -dot(right,    eye);
		view[3][1] = -dot(cameraUp, eye);
		view[3][2] =  dot(forward,  eye);
		return view;
	}

	mat4 	translate(const mat4& m, const vec3& offset)
	{
		mat4 result(m);

		for (int row = 0; row < 4; ++row)
			result[3][row] = m[0][row] * offset.x
				+ m[1][row] * offset.y
				+ m[2][row] * offset.z
				+ m[3][row];
		return result;
	}

	mat4	scale(const mat4& m, const vec3& factors)
	{
		mat4 result(m);

		for (int row = 0; row < 4; ++row)
		{
			result[0][row] = m[0][row] * factors.x;
			result[1][row] = m[1][row] * factors.y;
			result[2][row] = m[2][row] * factors.z;
		}
		return result;
	}

	mat4	rotate(const mat4& m, float angle, const vec3& axis)
	{
		const vec3 nAxis      = normalize(axis);
		const float cosAngle  = std::cos(angle);
		const float sinAngle  = std::sin(angle);
		const float oneMinusCos = 1.0f - cosAngle;

		mat4 rotation(0.0f);

		rotation[0][0] = cosAngle + oneMinusCos * nAxis.x * nAxis.x;
		rotation[0][1] = oneMinusCos * nAxis.x * nAxis.y + sinAngle * nAxis.z;
		rotation[0][2] = oneMinusCos * nAxis.x * nAxis.z - sinAngle * nAxis.y;

		rotation[1][0] = oneMinusCos * nAxis.y * nAxis.x - sinAngle * nAxis.z;
		rotation[1][1] = cosAngle + oneMinusCos * nAxis.y * nAxis.y;
		rotation[1][2] = oneMinusCos * nAxis.y * nAxis.z + sinAngle * nAxis.x;

		rotation[2][0] = oneMinusCos * nAxis.z * nAxis.x + sinAngle * nAxis.y;
		rotation[2][1] = oneMinusCos * nAxis.z * nAxis.y - sinAngle * nAxis.x;
		rotation[2][2] = cosAngle + oneMinusCos * nAxis.z * nAxis.z;
		rotation[3][3] = 1.0f;

		mat4 result;
		for (int col = 0; col < 3; ++col)
			for (int row = 0; row < 4; ++row)
				result[col][row] = m[0][row] * rotation[0][col]
					+ m[1][row] * rotation[1][col]
					+ m[2][row] * rotation[2][col];

		for (int row = 0; row < 4; ++row)
			result[3][row] = m[3][row];

		return result;
	}



}
