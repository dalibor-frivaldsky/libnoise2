#pragma once




namespace noise
{

	namespace math
	{
	
		template< typename ValueType >
		class Interpolations;

	}
	
}

#if defined( LIBNOISE2_AT_LEAST_SSE2 )
	#include <noise/math/Interpolations_Vector4.hpp>
#else
	#include <noise/math/Interpolations_Scalar.hpp>
#endif
