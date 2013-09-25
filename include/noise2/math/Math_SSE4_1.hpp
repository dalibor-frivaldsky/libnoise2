#pragma once


#include <cmath>


#include <smmintrin.h>


#include <noise2/math/Math_SSSE3.hpp>




namespace noise2
{

	namespace math
	{

		template< typename ValueType >
		class Math_SSE4_1;



		template<>
		class Math_SSE4_1< float >: public Math_SSSE3< float >
		{

		public:

			// Arithmetic operations
			using Math_SSSE3::multiply;

			static inline
			Vector4I
			multiply( const Vector4I& l, const Vector4I& r )
			{
				return _mm_mullo_epi32( l, r );
			}


			// Misc operations
			using Math_SSSE3::blend;

			static inline
			Vector4F
			blend( const Vector4F& a, const Vector4F& b, const Vector4I blendMask )
			{
				return _mm_blendv_ps( a, b, _mm_castsi128_ps( blendMask ) );
			}

		};



		template<>
		class Math_SSE4_1< double >: public Math_SSSE3< double >
		{

		public:

			// Arithmetic operations
			using Math_SSSE3::multiply;

			static inline
			Vector4I
			multiply( const Vector4I& l, const Vector4I& r )
			{
				return _mm_mullo_epi32( l, r );
			}

		};

	}

}
