#pragma once


#include <cmath>


#include <smmintrin.h>


#include <noise/math/Math_SSSE3.hpp>




namespace noise
{

	namespace math
	{

		template< typename ValueType >
		class Math_SSE4_1;



		template<>
		class Math_SSE4_1< float >: public Math_SSSE3< float >
		{

		public:

			// Misc operations
			using Math_SSSE3::blend;

			static inline
			Vector4F
			blend( const Vector4F& a, const Vector4F& b, const Vector4I blendMask )
			{
				return _mm_blendv_ps( a, b, blendMask );
			}

		};

	}

}
