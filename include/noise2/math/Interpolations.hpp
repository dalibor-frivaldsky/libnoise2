#pragma once




namespace noise2
{

	namespace math
	{
	
		template< typename ValueType >
		class Interpolations;

	}
	
}

#if defined( LIBNOISE2_AT_LEAST_SSE2 )
	#include <noise2/math/Interpolations_Vector4.hpp>
#else
	#include <noise2/math/Interpolations_Scalar.hpp>
#endif
