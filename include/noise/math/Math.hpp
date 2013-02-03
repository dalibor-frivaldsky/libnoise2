#pragma once


#include <noise/Platform.hpp>


#include <noise/math/MathBase.hpp>
#define LIBNOISE2_SUPPORT_MATH_IMPL		MathBase

#if defined( LIBNOISE2_AT_LEAST_SSE1 )
#include <noise/math/Math_SSE1.hpp>
#undef LIBNOISE2_SUPPORT_MATH_IMPL
#define LIBNOISE2_SUPPORT_MATH_IMPL		Math_SSE1
#endif

#if defined( LIBNOISE2_AT_LEAST_SSE2 )
#include <noise/math/Math_SSE2.hpp>
#undef LIBNOISE2_SUPPORT_MATH_IMPL
#define LIBNOISE2_SUPPORT_MATH_IMPL		Math_SSE2
#endif

#if defined( LIBNOISE2_AT_LEAST_SSE3 )
#include <noise/math/Math_SSE3.hpp>
#undef LIBNOISE2_SUPPORT_MATH_IMPL
#define LIBNOISE2_SUPPORT_MATH_IMPL		Math_SSE3
#endif

namespace noise
{
	
	namespace math
	{

		template< typename ValueType >
		class Math: public LIBNOISE2_SUPPORT_MATH_IMPL< ValueType >
		{
		};

	}
	
}
