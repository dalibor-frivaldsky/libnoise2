#pragma once


#include <cmath>


#include <emmintrin.h>


#include <noise/math/Math_SSE1.hpp>




namespace noise
{

	namespace math
	{

		template< typename ValueType >
		class Math_SSE2;



		template<>
		class Math_SSE2< float >: public Math_SSE1< float >
		{

		public:
		
			typedef __m128i			Vector2I;
			typedef __m128i			Vector3I;
			typedef __m128i			Vector4I;



		public:

			// Memory operations
			using Math_SSE1::loadFromMemory;
			using Math_SSE1::storeToMemory;

			static inline
			Vector4I
			loadFromMemory( const ScalarI* memoryLocation )
			{
				 return _mm_load_si128( (const Vector4I*) memoryLocation );
			}

			static inline
			void
			storeToMemory( ScalarI* memoryLocation, const Vector4I& v )
			{
				_mm_store_si128( (Vector4I*) memoryLocation, v );
			}

			static inline
			Vector4I
			loadFromMemory( const ScalarUI* memoryLocation )
			{
				 return _mm_load_si128( (const Vector4I*) memoryLocation );
			}

			static inline
			void
			storeToMemory( ScalarUI* memoryLocation, const Vector4I& v )
			{
				_mm_store_si128( (Vector4I*) memoryLocation, v );
			}


			// Vectorization
			using Math_SSE1::vectorizeOne;

			static inline
			Vector4I
			vectorizeOne( const ScalarI& i )
			{
				return _mm_set1_epi32( i );
			}

			static inline
			Vector4I
			vectorizeOne( const ScalarUI& ui )
			{
				return _mm_set1_epi32( ui );
			}
		
			static inline
			Vector2I
			vectorize( const ScalarI& x, const ScalarI& y )
			{
				return _mm_set_epi32( 0, 0, y, x );
			}
		
			static inline
			Vector3I
			vectorize( const ScalarI& x, const ScalarI& y, const ScalarI& z )
			{
				return _mm_set_epi32( 0, z, y, x );
			}
		
			static inline
			Vector4I
			vectorize( const ScalarI& x, const ScalarI& y, const ScalarI& z, const ScalarI& w )
			{
				return _mm_set_epi32( w, z, y, x );
			}


			// Conversions
			static inline
			Vector4I
			floatToIntTruncated( const Vector4F& v )
			{
				return _mm_cvttps_epi32( v );
			}
		
			static inline
			Vector4F
			intToFloat( const Vector4I& v )
			{
				return _mm_cvtepi32_ps( v );
			}


			// Arithmetic operations
			using Math_SSE1::multiply;
			using Math_SSE1::add;
			using Math_SSE1::subtract;

			static inline
			Vector4I
			multiply( const Vector4I& l, const Vector4I& r )
			{
				Vector4I	tmp1 = _mm_mul_epu32( l, r );
				Vector4I	tmp2 = _mm_mul_epu32( _mm_srli_si128( l, 4 ), _mm_srli_si128( r, 4 ) );
				return _mm_unpacklo_epi32( _mm_shuffle_epi32( tmp1, _MM_SHUFFLE( 0, 0, 2, 0 ) ),
										   _mm_shuffle_epi32( tmp2, _MM_SHUFFLE( 0, 0, 2, 0 ) ) );
			}

			static inline
			Vector4I
			add( const Vector4I& l, const Vector4I& r )
			{
				return _mm_add_epi32( l, r );
			}

			static inline
			Vector4I
			subtract( const Vector4I& l, const Vector4I& r )
			{
				return _mm_sub_epi32( l, r );
			}


			// Comparison
			static inline
			bool
			isAllZeros( const Vector4I& v )
			{
    			return _mm_movemask_epi8( _mm_cmpeq_epi8( v, _mm_setzero_si128() ) ) == 0xFFFF;
			}


			// Shift operations
			static inline
			Vector4I
			shiftRightLogical( const Vector4F& v, int bitCount )
			{
				return _mm_srli_epi32( _mm_castps_si128( v ), bitCount );
			}

			static inline
			Vector4I
			shiftRightLogical( const Vector4I& v, int bitCount )
			{
				return _mm_srli_epi32( v, bitCount );
			}

			static inline
			Vector4I
			shiftRightLoop( const Vector4I& v, int count )
			{
				Vector4I	shiftedV = v;

				for( int i = 0; i < count; ++i )
				{
					shiftedV = _mm_shuffle_epi32( shiftedV, _MM_SHUFFLE( 0, 3, 2, 1 ) );
				}

				return shiftedV;
			}

			static inline
			Vector4I
			shiftRightLoop( const Vector4F& v, int count )
			{
				return _mm_castsi128_ps( shiftRightLoop( _mm_castps_si128( v ), count ) );
			}


			// Exponential
			static inline
			Vector4F
			exp( const Vector4F& v )
			{
				static const float		exp_hi = 88.3762626647949f;
				static const float		exp_lo = -88.3762626647949f;

				static const float		cephes_log2ef = 1.44269504088896341f;
				static const float		cephes_exp_C1 = 0.693359375f;
				static const float		cephes_exp_C2 = -2.12194440e-4f;

				static const float		cephes_exp_p0 = 1.9875691500E-4f;
				static const float		cephes_exp_p1 = 1.3981999507E-3f;
				static const float		cephes_exp_p2 = 8.3334519073E-3f;
				static const float		cephes_exp_p3 = 4.1665795894E-2f;
				static const float		cephes_exp_p4 = 1.6666665459E-1f;
				static const float		cephes_exp_p5 = 5.0000001201E-1f;

				Vector4F	x = v;
				Vector4F	tmp = _mm_setzero_ps();
				Vector4F	fx;
				Vector4I	emm0i;
				Vector4F	one = _mm_set1_ps( 1.0f );

				x = _mm_min_ps( x, _mm_set1_ps( exp_hi ) );
				x = _mm_max_ps( x, _mm_set1_ps( exp_lo ) );

				/* express exp(x) as exp(g + n*log(2)) */
				fx = _mm_mul_ps( x, _mm_set1_ps( cephes_log2ef ) );
				fx = _mm_add_ps( fx, _mm_set1_ps( 0.5f ) );

				/* how to perform a floorf with SSE: just below */
				emm0i = _mm_cvttps_epi32( fx );
				tmp  = _mm_cvtepi32_ps( emm0i );

				/* if greater, substract 1 */
				Vector4F	mask = _mm_cmpgt_ps( tmp, fx );    
				mask = _mm_and_ps( mask, one );
				fx = _mm_sub_ps( tmp, mask );

				tmp = _mm_mul_ps( fx, _mm_set1_ps( cephes_exp_C1 ) );
				Vector4F	z = _mm_mul_ps( fx, _mm_set1_ps( cephes_exp_C2 ) );
				x = _mm_sub_ps( x, tmp );
				x = _mm_sub_ps( x, z );

				z = _mm_mul_ps( x, x );

				Vector4F	y = _mm_set1_ps( cephes_exp_p0 );
				y = _mm_mul_ps( y, x );
				y = _mm_add_ps( y, _mm_set1_ps( cephes_exp_p1 ) );
				y = _mm_mul_ps( y, x );
				y = _mm_add_ps( y, _mm_set1_ps( cephes_exp_p2 ) );
				y = _mm_mul_ps( y, x );
				y = _mm_add_ps( y, _mm_set1_ps( cephes_exp_p3 ) );
				y = _mm_mul_ps( y, x );
				y = _mm_add_ps( y, _mm_set1_ps( cephes_exp_p4 ) );
				y = _mm_mul_ps( y, x );
				y = _mm_add_ps( y, _mm_set1_ps( cephes_exp_p5 ) );
				y = _mm_mul_ps( y, z );
				y = _mm_add_ps( y, x );
				y = _mm_add_ps( y, one );

				/* build 2^n */
				emm0i = _mm_cvttps_epi32( fx) ;
				emm0i = _mm_add_epi32( emm0i, _mm_set1_epi32( 0x7f ) );
				emm0i = _mm_slli_epi32( emm0i, 23 );
				Vector4F	pow2n = _mm_castsi128_ps( emm0i );
				y = _mm_mul_ps( y, pow2n );

				return y;
			}


			// Trigonometric
			static inline
			Vector4F
			cos( const Vector4F& v )
			{
				static const int		sign_mask = 0x80000000;
				static const int		inv_sign_mask = ~0x80000000;

				static const float		minus_cephes_DP1 = -0.78515625f;
				static const float		minus_cephes_DP2 = -2.4187564849853515625e-4f;
				static const float		minus_cephes_DP3 = -3.77489497744594108e-8f;
				static const float		sincof_p0 = -1.9515295891E-4f;
				static const float		sincof_p1 = 8.3321608736E-3f;
				static const float		sincof_p2 = -1.6666654611E-1f;
				static const float		coscof_p0 = 2.443315711809948E-005f;
				static const float		coscof_p1 = -1.388731625493765E-003f;
				static const float		coscof_p2 = 4.166664568298827E-002f;
				static const float		cephes_FOPI = 1.27323954473516f;

				Vector4F	x = v;
				Vector4F	xmm1;
				Vector4F	xmm2 = _mm_setzero_ps();
				Vector4F	xmm3;
				Vector4F	y;
				Vector4I	emm0;
				Vector4I	emm2;

				/* take the absolute value */
				x = _mm_and_ps( x, _mm_set1_ps( *((float*)((void*)&inv_sign_mask)) ) );

				/* scale by 4/Pi */
				y = _mm_mul_ps( x, _mm_set1_ps( cephes_FOPI ) );

				/* store the integer part of y in mm0 */
				emm2 = _mm_cvttps_epi32( y );
				/* j=(j+1) & (~1) (see the cephes sources) */
				emm2 = _mm_add_epi32( emm2, _mm_set1_epi32( 1 ) );
				emm2 = _mm_and_si128( emm2, _mm_set1_epi32( ~1 ) );
				y = _mm_cvtepi32_ps( emm2 );

				emm2 = _mm_sub_epi32( emm2, _mm_set1_epi32( 2 ) );

				/* get the swap sign flag */
				emm0 = _mm_andnot_si128( emm2, _mm_set1_epi32( 4 ) );
				emm0 = _mm_slli_epi32( emm0, 29 );

				/* get the polynom selection mask */
				emm2 = _mm_and_si128( emm2, _mm_set1_epi32( 2 ));
				emm2 = _mm_cmpeq_epi32( emm2, _mm_setzero_si128() );

				Vector4F	signBit = _mm_castsi128_ps( emm0 );
				Vector4F	polyMask = _mm_castsi128_ps( emm2 );

				/* The magic pass: "Extended precision modular arithmetic" 
				x = ((x - y * DP1) - y * DP2) - y * DP3; */
				xmm1 = _mm_set1_ps( minus_cephes_DP1 );
				xmm2 = _mm_set1_ps( minus_cephes_DP2 );
				xmm3 = _mm_set1_ps( minus_cephes_DP3 );
				xmm1 = _mm_mul_ps( y, xmm1 );
				xmm2 = _mm_mul_ps( y, xmm2 );
				xmm3 = _mm_mul_ps( y, xmm3 );
				x = _mm_add_ps( x, xmm1 );
				x = _mm_add_ps( x, xmm2 );
				x = _mm_add_ps( x, xmm3 );

				/* Evaluate the first polynom  (0 <= x <= Pi/4) */
				Vector4F	z = _mm_mul_ps( x, x );
				y = _mm_set1_ps( coscof_p0 );

				y = _mm_mul_ps( y, z );
				y = _mm_add_ps( y, _mm_set1_ps( coscof_p1 ) );
				y = _mm_mul_ps( y, z );
				y = _mm_add_ps( y, _mm_set1_ps( coscof_p2 ) );
				y = _mm_mul_ps( y, z );
				y = _mm_mul_ps( y, z );
				Vector4F	tmp = _mm_mul_ps( z, _mm_set1_ps( 0.5f ) );
				y = _mm_sub_ps( y, tmp );
				y = _mm_add_ps( y, _mm_set1_ps( 1.0f ) );

				/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

				Vector4F	y2 = _mm_set1_ps( sincof_p0 );
				y2 = _mm_mul_ps( y2, z );
				y2 = _mm_add_ps( y2, _mm_set1_ps( sincof_p1 ) );
				y2 = _mm_mul_ps( y2, z );
				y2 = _mm_add_ps( y2, _mm_set1_ps( sincof_p2 ) );
				y2 = _mm_mul_ps( y2, z );
				y2 = _mm_mul_ps( y2, x );
				y2 = _mm_add_ps( y2, x );

				/* select the correct result from the two polynoms */  
				xmm3 = polyMask;
				y2 = _mm_and_ps( xmm3, y2 ); //, xmm3);
				y = _mm_andnot_ps( xmm3, y );
				y = _mm_add_ps( y, y2 );
				/* update the sign */
				y = _mm_xor_ps( y, signBit );

				return y;
			}

			static inline
			void
			sinCos( const Vector4F& v, Vector4F& sinV, Vector4F& cosV )
			{
				static const int		sign_mask = 0x80000000;
				static const int		inv_sign_mask = ~0x80000000;

				static const float		minus_cephes_DP1 = -0.78515625f;
				static const float		minus_cephes_DP2 = -2.4187564849853515625e-4f;
				static const float		minus_cephes_DP3 = -3.77489497744594108e-8f;
				static const float		sincof_p0 = -1.9515295891E-4f;
				static const float		sincof_p1 = 8.3321608736E-3f;
				static const float		sincof_p2 = -1.6666654611E-1f;
				static const float		coscof_p0 = 2.443315711809948E-005f;
				static const float		coscof_p1 = -1.388731625493765E-003f;
				static const float		coscof_p2 = 4.166664568298827E-002f;
				static const float		cephes_FOPI = 1.27323954473516f;

				Vector4F	x = v;
				Vector4F		xmm1;
				Vector4F		xmm2;
				Vector4F		xmm3 = _mm_setzero_ps();
				Vector4F		signBitSin;
				Vector4F		y;
				Vector4I		emm0;
				Vector4I		emm2;
				Vector4I		emm4;

				signBitSin = x;
				x = _mm_and_ps( x, _mm_set1_ps( *((float*)((void*)&inv_sign_mask)) ) );
				signBitSin = _mm_and_ps( signBitSin, _mm_set1_ps( *((float*)((void*)&sign_mask)) ) );

				y = _mm_mul_ps( x, _mm_set1_ps( cephes_FOPI ) );

				emm2 = _mm_cvttps_epi32( y );

				emm2 = _mm_add_epi32( emm2, _mm_set1_epi32( 1 ) );
				emm2 = _mm_and_si128( emm2, _mm_set1_epi32( ~1 ) );
				y = _mm_cvtepi32_ps( emm2 );

				emm4 = emm2;

				/* get the swap sign flag for the sseSin */
				emm0 = _mm_and_si128( emm2, _mm_set1_epi32( 4 ) );
				emm0 = _mm_slli_epi32( emm0, 29 );
				Vector4F	swapSignBitSin = _mm_castsi128_ps( emm0 );

				/* get the polynom selection mask for the sseSin*/
				emm2 = _mm_and_si128( emm2, _mm_set1_epi32( 2 ) );
				emm2 = _mm_cmpeq_epi32( emm2, _mm_setzero_si128() );
				Vector4F	polyMask = _mm_castsi128_ps( emm2 );

				/* The magic pass: "Extended precision modular arithmetic" 
		 		   x = ((x - y * DP1) - y * DP2) - y * DP3; */
				xmm1 = _mm_set1_ps( minus_cephes_DP1 );
				xmm2 = _mm_set1_ps( minus_cephes_DP2 );
				xmm3 = _mm_set1_ps( minus_cephes_DP3 );
				xmm1 = _mm_mul_ps( y, xmm1 );
				xmm2 = _mm_mul_ps( y, xmm2 );
				xmm3 = _mm_mul_ps( y, xmm3 );
				x = _mm_add_ps( x, xmm1 );
				x = _mm_add_ps( x, xmm2 );
				x = _mm_add_ps( x, xmm3 );

				emm4 = _mm_sub_epi32( emm4, _mm_set1_epi32( 2 ) );
				emm4 = _mm_andnot_si128( emm4, _mm_set1_epi32( 4 ) );
				emm4 = _mm_slli_epi32( emm4, 29 );
				Vector4F	signBitCos = _mm_castsi128_ps( emm4 );

				signBitSin = _mm_xor_ps( signBitSin, swapSignBitSin );

				/* Evaluate the first polynom  (0 <= x <= Pi/4) */
				Vector4F	z = _mm_mul_ps( x, x );
				y = _mm_set1_ps( coscof_p0 );

				y = _mm_mul_ps( y, z );
				y = _mm_add_ps( y, _mm_set1_ps( coscof_p1 ) );
				y = _mm_mul_ps( y, z );
				y = _mm_add_ps( y, _mm_set1_ps( coscof_p2 ) );
				y = _mm_mul_ps( y, z );
				y = _mm_mul_ps( y, z );
				Vector4F	tmp = _mm_mul_ps( z, _mm_set1_ps( 0.5f ) );
				y = _mm_sub_ps( y, tmp );
				y = _mm_add_ps( y, _mm_set1_ps( 1.0f ) );

				/* Evaluate the second polynom  (Pi/4 <= x <= 0) */
				Vector4F	y2 = _mm_set1_ps( sincof_p0 );
				y2 = _mm_mul_ps( y2, z );
				y2 = _mm_add_ps( y2, _mm_set1_ps( sincof_p1 ) );
				y2 = _mm_mul_ps( y2, z );
				y2 = _mm_add_ps( y2, _mm_set1_ps( sincof_p2 ) );
				y2 = _mm_mul_ps( y2, z );
				y2 = _mm_mul_ps( y2, x );
				y2 = _mm_add_ps( y2, x );

				/* select the correct result from the two polynoms */  
				xmm3 = polyMask;
				Vector4F	ysin2 = _mm_and_ps( xmm3, y2 );
				Vector4F	ysin1 = _mm_andnot_ps( xmm3, y );
				y2 = _mm_sub_ps( y2, ysin2 );
				y = _mm_sub_ps( y, ysin1 );

				xmm1 = _mm_add_ps( ysin1, ysin2 );
				xmm2 = _mm_add_ps( y, y2 );

				/* update the sign */
				sinV = _mm_xor_ps( xmm1, signBitSin );
				cosV = _mm_xor_ps( xmm2, signBitCos );
			}

		};

	}

}
