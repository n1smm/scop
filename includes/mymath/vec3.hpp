#ifndef VEC3_H
#define VEC3_H
namespace mymath {
	struct vec3 {
		float	x,y,z;
		vec3() : x(0.0f), y(0.0f), z(0.0f) {}
		vec3(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ) {}
	};
   vec3  operator+(const vec3& a, const vec3& b);
   vec3  operator-(const vec3& a, const vec3& b);
   vec3  operator*(const vec3& v, float s);
   vec3& operator+=(vec3& a, const vec3& b);
   vec3& operator-=(vec3& a, const vec3& b);
   vec3& operator*=(vec3& v, float s);

   vec3  operator*(float s, const vec3& v);
   vec3  operator-(const vec3& v);
   bool  operator==(const vec3&, const vec3&);
   bool  operator!=(const vec3&, const vec3&);
}

#endif
