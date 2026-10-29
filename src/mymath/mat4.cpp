#include "mymath/mat4.hpp"

namespace mymath {

	// identity
	mat4::mat4()
	{
		for (size_t col = 0; col < 4; ++col)
			for (size_t row = 0; row < 4; ++row)
				m[col][row] = (col == row) ? 1.0f : 0.0f;
	}

	//diagonal
	mat4::mat4(float s)
	{
		for (size_t col = 0; col < 4; ++col)
			for (size_t row = 0; row < 4; ++row)
				m[col][row] = (col == row) ? s : 0.0f;
	}
}
