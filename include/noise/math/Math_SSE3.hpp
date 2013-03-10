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

		public:

			// Arithmetic operations
			using Math_SSE2::reduce;

			static inline
			Vector4F
			reduce( const Vector4F& l, const Vector4F& r )
			{
				return _mm_hadd_ps( l, r );
			}

		};

		template<>
		class Math_SSE3< double >: public Math_SSE2< double >
		{

		public:

			// Arithmetic operations
			using Math_SSE2::reduce;
			
			static inline
			Vector4F
			reduce( const Vector4F& l, const Vector4F& r )
			{
				Vector4F	v;
				v.lo = _mm_hadd_pd( l.lo, l.hi );
				v.hi = _mm_hadd_pd( r.lo, r.hi );
				return v;
			}

		};

	}

}
