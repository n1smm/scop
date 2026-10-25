#ifndef MAT4_H
#define MAT4_H

#include <cstddef>

namespace mymath {

	struct mat4 {
		float m[4][4];

		mat4();
		explicit mat4(float s);

		float*       operator[](size_t col)        { return m[col]; }
		const float* operator[](size_t col) const  { return m[col]; }
	};


}

#endif

