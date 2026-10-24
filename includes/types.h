#ifndef TYPES_H
#define TYPES_H
#include <cstdint>
#include <cstddef>


namespace type {
using	int32	= std::int32_t;
using	uint	= std::uint32_t;
using	usize	= std::size_t;

}

static_assert(sizeof(float) == 4, "float must be 32 bits");
static_assert(sizeof(double) == 8, "double must be 64 bits");
static_assert(sizeof(bool) == 1, "bool must be 8 bit");

#endif
