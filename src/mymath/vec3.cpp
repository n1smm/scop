#include "mymath/vec3.hpp"

namespace mymath {
	vec3	operator+(const vec3& a, const vec3& b)
	{
		return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	vec3	operator-(const vec3& a, const vec3& b)
	{
		return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	vec3	operator*(const vec3& v, float s)
	{
		return vec3(v.x * s, v.y * s, v.z * s);
	}

	vec3	operator*(float s, const vec3& v)
	{
		return v * s;
	}

	vec3	operator-(const vec3& v)
	{
		return vec3(-v.x, -v.y, -v.z);
	}

	vec3&	operator+=(vec3& a, const vec3& b)
	{
		a.x += b.x; a.y += b.y; a.z += b.z;
		return a;
	}

	vec3&	operator-=(vec3& a, const vec3& b)
	{
		a.x -= b.x; a.y -= b.y; a.z -= b.z;
		return a;
	}

	vec3&	operator*=(vec3& v, float s)
	{
		v.x *= s; v.y *= s; v.z *= s;
		return v;
	}

	bool	operator==(const vec3& a, const vec3& b)
	{
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}

	bool	operator!=(const vec3& a, const vec3& b)
	{
		return !(a == b);
	}
}
