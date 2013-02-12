#pragma once


#include <cmath>


#include <tmmintrin.h>


#include <noise/math/Math_SSE3.hpp>




namespace noise
{

	namespace math
	{

		template< typename ValueType >
		class Math_SSSE3;



		template<>
		class Math_SSSE3< float >: public Math_SSE3< float >
		{
		};

		template<>
		class Math_SSSE3< double >: public Math_SSE3< double >
		{
		};

	}

}
