#pragma once


#include <noise2/Platform.hpp>


#include <noise2/math/MathBase.hpp>
#define LIBNOISE2_SUPPORT_MATH_IMPL		MathBase

#if defined( LIBNOISE2_AT_LEAST_SSE1 )
#include <noise2/math/Math_SSE1.hpp>
#undef LIBNOISE2_SUPPORT_MATH_IMPL
#define LIBNOISE2_SUPPORT_MATH_IMPL		Math_SSE1
#endif

#if defined( LIBNOISE2_AT_LEAST_SSE2 )
#include <noise2/math/Math_SSE2.hpp>
#undef LIBNOISE2_SUPPORT_MATH_IMPL
#define LIBNOISE2_SUPPORT_MATH_IMPL		Math_SSE2
#endif

#if defined( LIBNOISE2_AT_LEAST_SSE3 )
#include <noise2/math/Math_SSE3.hpp>
#undef LIBNOISE2_SUPPORT_MATH_IMPL
#define LIBNOISE2_SUPPORT_MATH_IMPL		Math_SSE3
#endif

#if defined( LIBNOISE2_AT_LEAST_SSSE3 )
#include <noise2/math/Math_SSSE3.hpp>
#undef LIBNOISE2_SUPPORT_MATH_IMPL
#define LIBNOISE2_SUPPORT_MATH_IMPL		Math_SSSE3
#endif

#if defined( LIBNOISE2_AT_LEAST_SSE4_1 )
#include <noise2/math/Math_SSE4_1.hpp>
#undef LIBNOISE2_SUPPORT_MATH_IMPL
#define LIBNOISE2_SUPPORT_MATH_IMPL		Math_SSE4_1
#endif

namespace noise2
{
	
	namespace math
	{

		template< typename ValueType >
		class Math: public LIBNOISE2_SUPPORT_MATH_IMPL< ValueType >
		{
		};

	}
	
}
