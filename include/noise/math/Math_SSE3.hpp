#pragma once


#include <cmath>


#include <pmmintrin.h>


#include <noise/math/Math_SSE2.hpp>




namespace noise
{

	namespace math
	{

		template< typename ValueType >
		class Math_SSE3;



		template<>
		class Math_SSE3< float >: public Math_SSE2< float >
		{
		};

	}

}
