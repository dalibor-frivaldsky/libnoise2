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



		template< typename ValueType >
		class Math_SSE2_Integer: public Math_SSE1< ValueType >
		{

		public:
		
			using typename Math_SSE1< ValueType >::ScalarF;
			using typename Math_SSE1< ValueType >::ScalarI;
			using typename Math_SSE1< ValueType >::ScalarUI;

			typedef __m128i			Vector4I;



		public:

			// Memory operations
			using Math_SSE1< ValueType >::loadFromMemory;
			using Math_SSE1< ValueType >::storeToMemory;

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


			// Constants
			static inline
			Vector4I
			constFullMaskI()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( uint32, fullMaskA, 0xffffffff );
				return loadFromMemory( fullMaskA );
			}

			static inline
			Vector4I
			constZeroI()
			{
				return _mm_setzero_si128();
			}

			static inline
			Vector4I
			constOneI()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( int32, oneA, 1 );
				return loadFromMemory( oneA );
			}

			static inline
			Vector4I
			constInvOneI()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( int32, oneA, ~1 );
				return loadFromMemory( oneA );
			}

			static inline
			Vector4I
			constTwoI()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( int32, twoA, 2 );
				return loadFromMemory( twoA );
			}

			static inline
			Vector4I
			constFourI()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( int32, fourA, 4 );
				return loadFromMemory( fourA );
			}


			// Vectorization
			using Math_SSE1< ValueType >::vectorizeOne;
			using Math_SSE1< ValueType >::vectorize;

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
			Vector4I
			vectorize( const ScalarI& x, const ScalarI& y )
			{
				return _mm_set_epi32( 0, 0, y, x );
			}
		
			static inline
			Vector4I
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


			// Shuffle operations
			using Math_SSE1< ValueType >::shuffle;

			template< uint8 First, uint8 Second, uint8 Third, uint8 Fourth >
			static inline
			Vector4I
			shuffle( const Vector4I& v )
			{
				return _mm_shuffle_epi32( v, _MM_SHUFFLE( Fourth, Third, Second, First ) );
			}


			// Arithmetic operations
			using Math_SSE1< ValueType >::multiply;
			using Math_SSE1< ValueType >::add;
			using Math_SSE1< ValueType >::subtract;

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
			using Math_SSE1< ValueType >::equal;
			using Math_SSE1< ValueType >::greaterThan;

			static inline
			bool
			isAllZeros( const Vector4I& v )
			{
    			return _mm_movemask_epi8( _mm_cmpeq_epi8( v, _mm_setzero_si128() ) ) == 0xFFFF;
			}

			static inline
			Vector4I
			equal( const Vector4I& l, const Vector4I& r )
			{
				return _mm_cmpeq_epi32( l, r );
			}

			static inline
			Vector4I
			greaterThan( const Vector4I& l, const Vector4I& r )
			{
				return _mm_cmpgt_epi32( l, r );
			}


			// Logical
			static inline
			Vector4I
			bitAnd( const Vector4I& l, const Vector4I& r )
			{
				return _mm_and_si128( l, r );
			}

			static inline
			Vector4I
			bitOr( const Vector4I& l, const Vector4I& r )
			{
				return _mm_or_si128( l, r );
			}

			static inline
			Vector4I
			bitXor( const Vector4I& l, const Vector4I& r )
			{
				return _mm_xor_si128( l, r );
			}


			// Shift operations
			using Math_SSE1< ValueType >::shiftRightLoop;
			using Math_SSE1< ValueType >::shiftLeftLoop;

			static inline
			Vector4I
			shiftRightLogical( const Vector4I& v, int bitCount )
			{
				return _mm_srli_epi32( v, bitCount );
			}

			static inline
			Vector4I
			shiftLeftLogical( const Vector4I& v, int bitCount )
			{
				return _mm_slli_epi32( v, bitCount );
			}

			static inline
			Vector4I
			shiftRightLoop( const Vector4I& v, int count )
			{
				Vector4I	shiftedV = v;

				for( int i = 0; i < count; ++i )
				{
					shiftedV = shuffle< 3, 0, 1, 2 >( shiftedV );
				}

				return shiftedV;
			}

			static inline
			Vector4I
			shiftLeftLoop( const Vector4I& v, int count )
			{
				Vector4I	shiftedV = v;

				for( int i = 0; i < count; ++i )
				{
					shiftedV = shuffle< 1, 2, 3, 0 >( shiftedV );
				}

				return shiftedV;
			}

		};



		template<>
		class Math_SSE2< float >: public Math_SSE2_Integer< float >
		{

		public:

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

			// Casts
			static inline
			Vector4F
			castToFloat( const Vector4I& v )
			{
				return _mm_castsi128_ps( v );
			}

			static inline
			Vector4I
			castToInt( const Vector4F& v )
			{
				return _mm_castps_si128( v );
			}


			// Shift operations
			using Math_SSE2_Integer< float >::shiftRightLogical;

			static inline
			Vector4I
			shiftRightLogical( const Vector4F& v, int bitCount )
			{
				return shiftRightLogical( _mm_castps_si128( v ), bitCount );
			}

			

			// Misc operations
			static inline
			Vector4F
			blend( const Vector4F& a, const Vector4F& b, const Vector4I blendMask )
			{
				Vector4I	negMask = constFullMaskI();

				Vector4I	bPart = bitAnd( castToInt( b ), blendMask );
				Vector4I	negBlendMask = bitXor( blendMask, negMask );
				
				return castToFloat( add( bPart, bitAnd( castToInt( a ), negBlendMask ) ) );
			}

			static inline
			Vector4F
			select( const Vector4F& v, const Vector4I& mask )
			{
				return castToFloat( bitAnd( castToInt( v ), mask ) );
			}


			// Exponential
			static inline
			Vector4F
			exp( const Vector4F& v )
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, exp_hi, 88.3762626647949f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, exp_lo, -88.3762626647949f );
				
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, cephes_log2ef, 1.44269504088896341f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, cephes_exp_C1, 0.693359375f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, cephes_exp_C2, -2.12194440e-4f );
				
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, cephes_exp_p0, 1.9875691500E-4f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, cephes_exp_p1, 1.3981999507E-3f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, cephes_exp_p2, 8.3334519073E-3f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, cephes_exp_p3, 4.1665795894E-2f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, cephes_exp_p4, 1.6666665459E-1f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, cephes_exp_p5, 5.0000001201E-1f );

				LIBNOISE2_SUPPORT_CONST_ARRAY( int32, i127, 0x7f );
				
				Vector4F	x = v;
				Vector4F	tmp = constZeroF();
				Vector4F	fx;
				Vector4I	emm0i;
				Vector4F	one = constOneF();

				x = _mm_min_ps( x, loadFromMemory( exp_hi ) );
				x = _mm_max_ps( x, loadFromMemory( exp_lo ) );

				/* express exp(x) as exp(g + n*log(2)) */
				fx = _mm_mul_ps( x, loadFromMemory( cephes_log2ef ) );
				fx = _mm_add_ps( fx, constHalfF() );

				/* how to perform a floorf with SSE: just below */
				emm0i = _mm_cvttps_epi32( fx );
				tmp  = _mm_cvtepi32_ps( emm0i );

				/* if greater, substract 1 */
				Vector4F	mask = _mm_cmpgt_ps( tmp, fx );    
				mask = _mm_and_ps( mask, one );
				fx = _mm_sub_ps( tmp, mask );

				tmp = _mm_mul_ps( fx, loadFromMemory( cephes_exp_C1 ) );
				Vector4F	z = _mm_mul_ps( fx, loadFromMemory( cephes_exp_C2 ) );
				x = _mm_sub_ps( x, tmp );
				x = _mm_sub_ps( x, z );

				z = _mm_mul_ps( x, x );

				Vector4F	y = loadFromMemory( cephes_exp_p0 );
				y = _mm_mul_ps( y, x );
				y = _mm_add_ps( y, loadFromMemory( cephes_exp_p1 ) );
				y = _mm_mul_ps( y, x );
				y = _mm_add_ps( y, loadFromMemory( cephes_exp_p2 ) );
				y = _mm_mul_ps( y, x );
				y = _mm_add_ps( y, loadFromMemory( cephes_exp_p3 ) );
				y = _mm_mul_ps( y, x );
				y = _mm_add_ps( y, loadFromMemory( cephes_exp_p4 ) );
				y = _mm_mul_ps( y, x );
				y = _mm_add_ps( y, loadFromMemory( cephes_exp_p5 ) );
				y = _mm_mul_ps( y, z );
				y = _mm_add_ps( y, x );
				y = _mm_add_ps( y, one );

				/* build 2^n */
				emm0i = _mm_cvttps_epi32( fx) ;
				emm0i = _mm_add_epi32( emm0i, loadFromMemory( i127 ) );
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
				LIBNOISE2_SUPPORT_CONST_ARRAY( int, inv_sign_mask, ~0x80000000 );
				
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, minus_cephes_DP1, -0.78515625f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, minus_cephes_DP2, -2.4187564849853515625e-4f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, minus_cephes_DP3, -3.77489497744594108e-8f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, sincof_p0, -1.9515295891E-4f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, sincof_p1, 8.3321608736E-3f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, sincof_p2, -1.6666654611E-1f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, coscof_p0, 2.443315711809948E-005f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, coscof_p1, -1.388731625493765E-003f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, coscof_p2, 4.166664568298827E-002f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, cephes_FOPI, 1.27323954473516f );
				
				Vector4F	x = v;
				Vector4F	xmm1;
				Vector4F	xmm2 = constZeroF();
				Vector4F	xmm3;
				Vector4F	y;
				Vector4I	emm0;
				Vector4I	emm2;

				/* take the absolute value */
				x = _mm_and_ps( x, loadFromMemory( (ScalarI*) inv_sign_mask ) );
				
				/* scale by 4/Pi */
				y = _mm_mul_ps( x, loadFromMemory( cephes_FOPI ) );

				/* store the integer part of y in mm0 */
				emm2 = _mm_cvttps_epi32( y );
				/* j=(j+1) & (~1) (see the cephes sources) */
				emm2 = _mm_add_epi32( emm2, constOneI() );
				emm2 = _mm_and_si128( emm2, constInvOneI() );
				y = _mm_cvtepi32_ps( emm2 );

				emm2 = _mm_sub_epi32( emm2, constTwoI() );

				/* get the swap sign flag */
				emm0 = _mm_andnot_si128( emm2, constFourI() );
				emm0 = _mm_slli_epi32( emm0, 29 );

				/* get the polynom selection mask */
				emm2 = _mm_and_si128( emm2, constTwoI() );
				emm2 = _mm_cmpeq_epi32( emm2, constZeroI() );

				Vector4F	signBit = _mm_castsi128_ps( emm0 );
				Vector4F	polyMask = _mm_castsi128_ps( emm2 );

				/* The magic pass: "Extended precision modular arithmetic" 
				x = ((x - y * DP1) - y * DP2) - y * DP3; */
				xmm1 = loadFromMemory( minus_cephes_DP1 );
				xmm2 = loadFromMemory( minus_cephes_DP2 );
				xmm3 = loadFromMemory( minus_cephes_DP3 );
				xmm1 = _mm_mul_ps( y, xmm1 );
				xmm2 = _mm_mul_ps( y, xmm2 );
				xmm3 = _mm_mul_ps( y, xmm3 );
				x = _mm_add_ps( x, xmm1 );
				x = _mm_add_ps( x, xmm2 );
				x = _mm_add_ps( x, xmm3 );

				/* Evaluate the first polynom  (0 <= x <= Pi/4) */
				Vector4F	z = _mm_mul_ps( x, x );
				y = loadFromMemory( coscof_p0 );

				y = _mm_mul_ps( y, z );
				y = _mm_add_ps( y, loadFromMemory( coscof_p1 ) );
				y = _mm_mul_ps( y, z );
				y = _mm_add_ps( y, loadFromMemory( coscof_p2 ) );
				y = _mm_mul_ps( y, z );
				y = _mm_mul_ps( y, z );
				Vector4F	tmp = _mm_mul_ps( z, constHalfF() );
				y = _mm_sub_ps( y, tmp );
				y = _mm_add_ps( y, constOneF() );

				/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

				Vector4F	y2 = loadFromMemory( sincof_p0 );
				y2 = _mm_mul_ps( y2, z );
				y2 = _mm_add_ps( y2, loadFromMemory( sincof_p1 ) );
				y2 = _mm_mul_ps( y2, z );
				y2 = _mm_add_ps( y2, loadFromMemory( sincof_p2 ) );
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
				LIBNOISE2_SUPPORT_CONST_ARRAY( int, sign_mask, 0x80000000 );
				LIBNOISE2_SUPPORT_CONST_ARRAY( int, inv_sign_mask, ~0x80000000 );
				
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, minus_cephes_DP1, -0.78515625f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, minus_cephes_DP2, -2.4187564849853515625e-4f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, minus_cephes_DP3, -3.77489497744594108e-8f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, sincof_p0, -1.9515295891E-4f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, sincof_p1, 8.3321608736E-3f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, sincof_p2, -1.6666654611E-1f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, coscof_p0, 2.443315711809948E-005f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, coscof_p1, -1.388731625493765E-003f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, coscof_p2, 4.166664568298827E-002f );
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, cephes_FOPI, 1.27323954473516f );

				Vector4F	x = v;
				Vector4F	xmm1;
				Vector4F	xmm2;
				Vector4F	xmm3 = _mm_setzero_ps();
				Vector4F	signBitSin;
				Vector4F	y;
				Vector4I	emm0;
				Vector4I	emm2;
				Vector4I	emm4;

				signBitSin = x;
				x = _mm_and_ps( x, loadFromMemory( (ScalarI*) inv_sign_mask ) );
				signBitSin = _mm_and_ps( signBitSin, loadFromMemory( (ScalarI*) sign_mask ) );

				y = _mm_mul_ps( x, loadFromMemory( cephes_FOPI ) );

				emm2 = _mm_cvttps_epi32( y );

				emm2 = _mm_add_epi32( emm2, constOneI() );
				emm2 = _mm_and_si128( emm2, constInvOneI() );
				y = _mm_cvtepi32_ps( emm2 );

				emm4 = emm2;

				/* get the swap sign flag for the sseSin */
				emm0 = _mm_and_si128( emm2, constFourI() );
				emm0 = _mm_slli_epi32( emm0, 29 );
				Vector4F	swapSignBitSin = _mm_castsi128_ps( emm0 );

				/* get the polynom selection mask for the sseSin*/
				emm2 = _mm_and_si128( emm2, constTwoI() );
				emm2 = _mm_cmpeq_epi32( emm2, constZeroI() );
				Vector4F	polyMask = _mm_castsi128_ps( emm2 );

				/* The magic pass: "Extended precision modular arithmetic" 
		 		   x = ((x - y * DP1) - y * DP2) - y * DP3; */
				xmm1 = loadFromMemory( minus_cephes_DP1 );
				xmm2 = loadFromMemory( minus_cephes_DP2 );
				xmm3 = loadFromMemory( minus_cephes_DP3 );
				xmm1 = _mm_mul_ps( y, xmm1 );
				xmm2 = _mm_mul_ps( y, xmm2 );
				xmm3 = _mm_mul_ps( y, xmm3 );
				x = _mm_add_ps( x, xmm1 );
				x = _mm_add_ps( x, xmm2 );
				x = _mm_add_ps( x, xmm3 );

				emm4 = _mm_sub_epi32( emm4, constTwoI() );
				emm4 = _mm_andnot_si128( emm4, constFourI() );
				emm4 = _mm_slli_epi32( emm4, 29 );
				Vector4F	signBitCos = _mm_castsi128_ps( emm4 );

				signBitSin = _mm_xor_ps( signBitSin, swapSignBitSin );

				/* Evaluate the first polynom  (0 <= x <= Pi/4) */
				Vector4F	z = _mm_mul_ps( x, x );
				y = loadFromMemory( coscof_p0 );

				y = _mm_mul_ps( y, z );
				y = _mm_add_ps( y, loadFromMemory( coscof_p1 ) );
				y = _mm_mul_ps( y, z );
				y = _mm_add_ps( y, loadFromMemory( coscof_p2 ) );
				y = _mm_mul_ps( y, z );
				y = _mm_mul_ps( y, z );
				Vector4F	tmp = _mm_mul_ps( z, constHalfF() );
				y = _mm_sub_ps( y, tmp );
				y = _mm_add_ps( y, constOneF() );

				/* Evaluate the second polynom  (Pi/4 <= x <= 0) */
				Vector4F	y2 = loadFromMemory( sincof_p0 );
				y2 = _mm_mul_ps( y2, z );
				y2 = _mm_add_ps( y2, loadFromMemory( sincof_p1 ) );
				y2 = _mm_mul_ps( y2, z );
				y2 = _mm_add_ps( y2, loadFromMemory( sincof_p2 ) );
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



		template<>
		class Math_SSE2< double >: public Math_SSE2_Integer< double >
		{

		public:
		
			typedef struct
			{
				__m128d	lo;
				__m128d	hi;
			}					Vector4F;



		public:

			// Memory operations
			using Math_SSE2_Integer< double >::loadFromMemory;
			using Math_SSE2_Integer< double >::storeToMemory;

			static inline
			Vector4F
			loadFromMemory( const ScalarF* memoryLocation )
			{
				Vector4F	v;
				v.lo = _mm_load_pd( memoryLocation );
				v.hi = _mm_load_pd( memoryLocation + 2 );
				return v;
			}

			static inline
			void
			storeToMemory( ScalarF* memoryLocation, const Vector4F& v )
			{
				_mm_store_pd( memoryLocation, v.lo );
				_mm_store_pd( memoryLocation + 2, v.hi );
			}


			// Constants
			static inline
			Vector4F
			constZeroF()
			{
				Vector4F	v;
				v.lo = _mm_setzero_pd();
				v.hi = _mm_setzero_pd();
				return v;
			}

			static inline
			Vector4F
			constHalfF()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( double, halfA, 0.5 );
				return loadFromMemory( halfA );
			}

			static inline
			Vector4F
			constOneF()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( double, oneA, 1.0 );
				return loadFromMemory( oneA );
			}

			static inline
			Vector4F
			constMinusOneF()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( double, minusOneA, -1.0 );
				return loadFromMemory( minusOneA );
			}

			static inline
			Vector4F
			constTwoF()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( double, twoA, 2.0 );
				return loadFromMemory( twoA );
			}

			static inline
			Vector4F
			constPiF()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( double, piA, 3.14159265358979323846 );
				return loadFromMemory( piA );
			}

			static inline
			Vector4F
			constMinusPiF()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( double, minusPiA, -3.14159265358979323846 );
				return loadFromMemory( minusPiA );
			}


			// Vectorization
			using Math_SSE2_Integer< double >::vectorizeOne;
			using Math_SSE2_Integer< double >::vectorize;

			static inline
			Vector4F
			vectorizeOne( const ScalarF& f )
			{
				Vector4F	v;
				v.lo = _mm_set1_pd( f );
				v.hi = _mm_set1_pd( f );
				return v;
			}
		
			static inline
			Vector4F
			vectorize( const ScalarF& x, const ScalarF& y )
			{
				Vector4F	v;
				v.lo = _mm_set_pd( y, x );
				v.hi = _mm_setzero_pd();
				return v;
			}
		
			static inline
			Vector4F
			vectorize( const ScalarF& x, const ScalarF& y, const ScalarF& z )
			{
				Vector4F	v;
				v.lo = _mm_set_pd( y, x );
				v.hi = _mm_set_pd( 0.0, z );
				return v;
			}
		
			static inline
			Vector4F
			vectorize( const ScalarF& x, const ScalarF& y, const ScalarF& z, const ScalarF& w )
			{
				Vector4F	v;
				v.lo = _mm_set_pd( y, x );
				v.hi = _mm_set_pd( w, z );
				return v;
			}


			// Shuffle operations
			using Math_SSE2_Integer< double >::shuffle;

			template< uint8 First, uint8 Second, uint8 Third, uint8 Fourth >
			static inline
			Vector4F
			shuffle( const Vector4F& a, const Vector4F& b )
			{
				VECTOR4_ALIGN( ScalarF	tmp[ 8 ] );
				storeToMemory( tmp, a );
				storeToMemory( tmp + 4, b );

				VECTOR4_ALIGN( ScalarF	res[ 4 ] );
				res[ 0 ] = tmp[ First ];
				res[ 1 ] = tmp[ Second ];
				res[ 2 ] = tmp[ Third + 4 ];
				res[ 3 ] = tmp[ Fourth + 4 ];

				return loadFromMemory( res );
			}


			// Arithmetic operations
			using Math_SSE2_Integer< double >::multiply;
			using Math_SSE2_Integer< double >::add;
			using Math_SSE2_Integer< double >::subtract;

			static inline
			Vector4F
			multiply( const Vector4F& l, const Vector4F& r )
			{
				Vector4F	v;
				v.lo = _mm_mul_pd( l.lo, r.lo );
				v.hi = _mm_mul_pd( l.hi, r.hi );
				return v;
			}

			static inline
			Vector4F
			divide( const Vector4F& l, const Vector4F& r )
			{
				Vector4F	v;
				v.lo = _mm_div_pd( l.lo, r.lo );
				v.hi = _mm_div_pd( l.hi, r.hi );
				return v;
			}
		
			static inline
			Vector4F
			add( const Vector4F& l, const Vector4F& r )
			{
				Vector4F	v;
				v.lo = _mm_add_pd( l.lo, r.lo );
				v.hi = _mm_add_pd( l.hi, r.hi );
				return v;
			}

			static inline
			Vector4F
			subtract( const Vector4F& l, const Vector4F& r )
			{
				Vector4F	v;
				v.lo = _mm_sub_pd( l.lo, r.lo );
				v.hi = _mm_sub_pd( l.hi, r.hi );
				return v;
			}

			// Comparison
			using Math_SSE2_Integer< double >::equal;
			using Math_SSE2_Integer< double >::greaterThan;

			static inline
			Vector4F
			equal( const Vector4F& l, const Vector4F& r )
			{
				Vector4F	v;
				v.lo = _mm_cmpeq_pd( l.lo, r.lo );
				v.hi = _mm_cmpeq_pd( l.hi, r.hi );
				return v;
			}

			static inline
			Vector4F
			greaterThan( const Vector4F& l, const Vector4F& r )
			{
				Vector4F	v;
				v.lo = _mm_cmpgt_pd( l.lo, r.lo );
				v.hi = _mm_cmpgt_pd( l.hi, r.hi );
				return v;
			}

			static inline
			Vector4F
			lowerThan( const Vector4F& l, const Vector4F& r )
			{
				Vector4F	v;
				v.lo = _mm_cmplt_pd( l.lo, r.lo );
				v.hi = _mm_cmplt_pd( l.hi, r.hi );
				return v;
			}


			// Shift operations
			using Math_SSE2_Integer< double >::shiftRightLoop;
			using Math_SSE2_Integer< double >::shiftLeftLoop;

			static inline
			Vector4F
			shiftRightLoop( const Vector4F& v, int count )
			{
				Vector4F	shiftedV = v;

				for( int i = 0; i < count; ++i )
				{
					shiftedV = shuffle< 3, 0, 1, 2 >( shiftedV, shiftedV );
				}

				return shiftedV;
			}

			static inline
			Vector4F
			shiftLeftLoop( const Vector4F& v, int count )
			{
				Vector4F	shiftedV = v;

				for( int i = 0; i < count; ++i )
				{
					shiftedV = shuffle< 1, 2, 3, 0 >( shiftedV, shiftedV );
				}

				return shiftedV;
			}


			// Distribution operations
			static inline
			Vector4F
			distribute1st( const Vector4F& v )
			{
				Vector4F	v_;
				v_.lo = _mm_shuffle_pd( v.lo, v.lo, _MM_SHUFFLE2( 0, 0 ) );
				v_.hi = _mm_shuffle_pd( v.lo, v.lo, _MM_SHUFFLE2( 0, 0 ) );
				return v_;
			}

			static inline
			Vector4F
			distribute2nd( const Vector4F& v )
			{
				Vector4F	v_;
				v_.lo = _mm_shuffle_pd( v.lo, v.lo, _MM_SHUFFLE2( 1, 1 ) );
				v_.hi = _mm_shuffle_pd( v.lo, v.lo, _MM_SHUFFLE2( 1, 1 ) );
				return v_;
			}

			static inline
			Vector4F
			distribute3rd( const Vector4F& v )
			{
				Vector4F	v_;
				v_.lo = _mm_shuffle_pd( v.hi, v.hi, _MM_SHUFFLE2( 0, 0 ) );
				v_.hi = _mm_shuffle_pd( v.hi, v.hi, _MM_SHUFFLE2( 0, 0 ) );
				return v_;
			}

			static inline
			Vector4F
			distribute4th( const Vector4F& v )
			{
				Vector4F	v_;
				v_.lo = _mm_shuffle_pd( v.hi, v.hi, _MM_SHUFFLE2( 1, 1 ) );
				v_.hi = _mm_shuffle_pd( v.hi, v.hi, _MM_SHUFFLE2( 1, 1 ) );
				return v_;
			}


			// Conversions
			static inline
			Vector4I
			floatToIntTruncated( const Vector4F& v )
			{
				Vector4I	tmp = _mm_cvttpd_epi32( v.hi );
				tmp = shuffle< 2, 3, 0, 1 >( tmp );
				tmp = add( tmp, _mm_cvttpd_epi32( v.lo ) );
				return tmp;
			}
		
			static inline
			Vector4F
			intToFloat( const Vector4I& v )
			{
				Vector4I	tmpI = v;
				Vector4F	tmpF;
				tmpF.lo = _mm_cvtepi32_pd( tmpI );
				tmpF.hi = _mm_cvtepi32_pd( shuffle< 2, 3, 0, 1 >( tmpI ) );
				return tmpF;
			}


			// Casts
			/*static inline
			Vector4F
			castToFloat( const Vector4I& v )
			{
				return _mm_castsi128_ps( v );
			}*/

			static inline
			Vector4I
			castToInt( const Vector4F& v )
			{
				Vector4I	intLo = shuffle< 0, 2, 1, 3 >( _mm_castpd_si128( v.lo ) );
				Vector4I	intHi = shuffle< 0, 2, 1, 3 >( _mm_castpd_si128( v.hi ) );
				Vector4I	packed = _mm_unpacklo_epi32( intLo, intHi );
				return shuffle< 0, 2, 1, 3 >( packed );
			}


			// Misc operations
			static inline
			Vector4F
			blend( const Vector4F& a, const Vector4F& b, const Vector4I blendMask )
			{
				Vector4F	v;

				Vector4I	loBlendMask = shuffle< 0, 0, 1, 1 >( blendMask );
				Vector4I	loNegBlendMask = bitXor( loBlendMask, constFullMaskI() );
				v.lo = bitAnd( _mm_castpd_si128( b.lo ), loBlendMask );
				v.lo = _mm_castsi128_pd( add( v.lo, bitAnd( _mm_castpd_si128( a.lo ), loNegBlendMask ) ) );

				Vector4I	hiBlendMask = shuffle< 2, 2, 3, 3 >( blendMask );
				Vector4I	hiNegBlendMask = bitXor( hiBlendMask, constFullMaskI() );
				v.hi = bitAnd( _mm_castpd_si128( b.hi ), hiBlendMask );
				v.hi = _mm_castsi128_pd( add( v.hi, bitAnd( _mm_castpd_si128( a.hi ), hiNegBlendMask ) ) );

				return v;
			}

		};

	}

}
