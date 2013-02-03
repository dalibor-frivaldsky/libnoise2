#pragma once


#include <cmath>


#include <xmmintrin.h>


#include <noise/math/MathBase.hpp>




namespace noise
{

	namespace math
	{

		template< typename ValueType >
		class Math_SSE1;



		template<>
		class Math_SSE1< float >: public MathBase< float >
		{

		public:
		
			typedef __m128			Vector2F;
			typedef __m128			Vector3F;
			typedef __m128			Vector4F;



		public:

			// Memory operations
			static inline
			Vector4F
			loadFromMemory( const ScalarF* memoryLocation )
			{
				return _mm_load_ps( memoryLocation );
			}

			static inline
			void
			storeToMemory( ScalarF* memoryLocation, const Vector4F& v )
			{
				_mm_store_ps( memoryLocation, v );
			}


			// Vectorization
			static inline
			Vector4F
			vectorizeOne( const ScalarF& f )
			{
				return _mm_set1_ps( f );
			}
		
			static inline
			Vector2F
			vectorize( const ScalarF& x, const ScalarF& y )
			{
				return _mm_set_ps( 0.0f, 0.0f, y, x );
			}
		
			static inline
			Vector3F
			vectorize( const ScalarF& x, const ScalarF& y, const ScalarF& z )
			{
				return _mm_set_ps( 0.0f, z, y, x );
			}
		
			static inline
			Vector4F
			vectorize( const ScalarF& x, const ScalarF& y, const ScalarF& z, const ScalarF& w )
			{
				return _mm_set_ps( w, z, y, x );
			}


			// Arithmetic operations
			static inline
			Vector4F
			multiply( const Vector4F& l, const Vector4F& r )
			{
				return _mm_mul_ps( l, r );
			}

			static inline
			Vector4F
			divide( const Vector4F& l, const Vector4F& r )
			{
				return _mm_div_ps( l, r );
			}
		
			static inline
			Vector4F
			add( const Vector4F& l, const Vector4F& r )
			{
				return _mm_add_ps( l, r );
			}

			static inline
			Vector4F
			subtract( const Vector4F& l, const Vector4F& r )
			{
				return _mm_sub_ps( l, r );
			}


			// Distribution operations
			static inline
			Vector4F
			vectorDistribute1st( const Vector4F& input )
			{
				return _mm_shuffle_ps( input, input, _MM_SHUFFLE( 0, 0, 0, 0 ) );
			}

			static inline
			Vector4F
			vectorDistribute2nd( const Vector4F& input )
			{
				return _mm_shuffle_ps( input, input, _MM_SHUFFLE( 1, 1, 1, 1 ) );
			}

			static inline
			Vector4F
			vectorDistribute3rd( const Vector4F& input )
			{
				return _mm_shuffle_ps( input, input, _MM_SHUFFLE( 2, 2, 2, 2 ) );
			}

			static inline
			Vector4F
			vectorDistribute4th( const Vector4F& input )
			{
				return _mm_shuffle_ps( input, input, _MM_SHUFFLE( 3, 3, 3, 3 ) );
			}

		};

	}

}
