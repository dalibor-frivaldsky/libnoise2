#pragma once


#include <cmath>


#include <noise2/BasicTypes.hpp>




namespace noise2
{

	namespace math
	{

		namespace impl
		{

			template< typename ValueType >
			class Cephes;

			template<>
			class Cephes< float >
			{

			public:

				static inline
				float
				exp( float v )
				{
					float exp_hi = 88.3762626647949f;
					float exp_lo = -88.3762626647949f;
					
					float cephes_log2ef = 1.44269504088896341f;
					float cephes_exp_C1 = 0.693359375f;
					float cephes_exp_C2 = -2.12194440e-4f;
					
					float cephes_exp_p0 = 1.9875691500E-4f;
					float cephes_exp_p1 = 1.3981999507E-3f;
					float cephes_exp_p2 = 8.3334519073E-3f;
					float cephes_exp_p3 = 4.1665795894E-2f;
					float cephes_exp_p4 = 1.6666665459E-1f;
					float cephes_exp_p5 = 5.0000001201E-1f;

					int32 i127 = 0x7f;
					
					float	x = v;
					float	tmp = 0.0f;
					float	fx;
					int32		emm0i;
					float	one = 1.0f;

					x = x > exp_hi ? exp_hi : x;
					x = x < exp_lo ? exp_lo : x;

					/* express exp(x) as exp(g + n*log(2)) */
					fx = x * cephes_log2ef;
					fx += 0.5f;

					fx = floor( fx );

					tmp = fx * cephes_exp_C1;
					float	z = fx * cephes_exp_C2;
					x -= tmp;
					x -= z;

					z = x * x;

					float	y = cephes_exp_p0;
					y *= x;
					y += cephes_exp_p1;
					y *= x;
					y += cephes_exp_p2;
					y *= x;
					y += cephes_exp_p3;
					y *= x;
					y += cephes_exp_p4;
					y *= x;
					y += cephes_exp_p5;
					y *= z;
					y += x;
					y += 1.0f;

					/* build 2^n */
					emm0i = int32( fx );
					emm0i += i127;
					emm0i <<= 23;
					float pow2n = *((float*)((void*)&emm0i));
					y *= pow2n;

					return y;
				}

				static inline
				float
				cos( float v )
				{
					uint32 inv_sign_mask = ~0x80000000;
					
					float minus_cephes_DP1 = -0.78515625f;
					float minus_cephes_DP2 = -2.4187564849853515625e-4f;
					float minus_cephes_DP3 = -3.77489497744594108e-8f;
					float sincof_p0 = -1.9515295891E-4f;
					float sincof_p1 = 8.3321608736E-3f;
					float sincof_p2 = -1.6666654611E-1f;
					float coscof_p0 = 2.443315711809948E-005f;
					float coscof_p1 = -1.388731625493765E-003f;
					float coscof_p2 = 4.166664568298827E-002f;
					float cephes_FOPI = 1.27323954473516f;
					
					float	x = v;
					float	xmm1;
					float	xmm2 = 0.0f;
					float	xmm3;
					float	y;
					int32	emm0;
					int32	emm2;

					/* take the absolute value */
					uint32	xBin = *((uint32*)((void*)&x));
					xBin = xBin & inv_sign_mask;
					x = *((float*)((void*)&xBin));

					/* scale by 4/Pi */
					y = x * cephes_FOPI;
					
					/* store the integer part of y in mm0 */
					emm2 = int32( y );
					/* j=(j+1) & (~1) (see the cephes sources) */
					emm2 += 1;
					emm2 = emm2 & ((uint32)~1);
					y = float( emm2 );

					emm2 -= 2;

					/* get the swap sign flag */
					emm0 = (~emm2) & ((uint32) 4);
					emm0 <<= 29;
					
					/* get the polynom selection mask */
					emm2 = emm2 & ((uint32) 2);
					emm2 = emm2 == 0 ? 0xffffffff : 0x0;
					
					uint32	signBit = emm0;
					uint32	polyMask = emm2;

					/* The magic pass: "Extended precision modular arithmetic" 
					x = ((x - y * DP1) - y * DP2) - y * DP3; */
					xmm1 = minus_cephes_DP1;
					xmm2 = minus_cephes_DP2;
					xmm3 = minus_cephes_DP3;
					xmm1 = y * xmm1;
					xmm2 = y * xmm2;
					xmm3 = y * xmm3;
					x += xmm1;
					x += xmm2;
					x += xmm3;

					/* Evaluate the first polynom  (0 <= x <= Pi/4) */
					float	z = x * x;
					y = coscof_p0;

					y *= z;
					y += coscof_p1;
					y *= z;
					y += coscof_p2;
					y *= z;
					y *= z;
					float	tmp = z * 0.5f;
					y -= tmp;
					y += 1.0f;

					/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

					float	y2 = sincof_p0;
					y2 *= z;
					y2 += sincof_p1;
					y2 *= z;
					y2 += sincof_p2;
					y2 *= z;
					y2 *= x;
					y2 += x;

					/* select the correct result from the two polynoms */  
					uint32 tmpMask = polyMask;
					uint32 y2Bin = tmpMask & *((uint32*)((void*)&y2));
					y2 = *((float*)((void*)&y2Bin));
					uint32 yBin = (~tmpMask) & *((uint32*)((void*)&y));
					y = *((float*)((void*)&yBin));
					y += y2;
					yBin = *((uint32*)((void*)&y)) ^ signBit;
					y = *((float*)((void*)&yBin));
					
					return y;
				}
				
				static inline
				void
				sinCos( float v, float& sin, float& cos )
				{
					uint32 sign_mask = 0x80000000;
					uint32 inv_sign_mask = ~0x80000000;
					
					float minus_cephes_DP1 = -0.78515625f;
					float minus_cephes_DP2 = -2.4187564849853515625e-4f;
					float minus_cephes_DP3 = -3.77489497744594108e-8f;
					float sincof_p0 = -1.9515295891E-4f;
					float sincof_p1 = 8.3321608736E-3f;
					float sincof_p2 = -1.6666654611E-1f;
					float coscof_p0 = 2.443315711809948E-005f;
					float coscof_p1 = -1.388731625493765E-003f;
					float coscof_p2 = 4.166664568298827E-002f;
					float cephes_FOPI = 1.27323954473516f;
					
					float	x = v;
					float	xmm1;
					float	xmm2 = 0.0f;
					float	xmm3;
					uint32		signBitSin;
					float	y;
					int32		emm0;
					int32		emm2;
					int32		emm4;

					/* take the absolute value */
					uint32	xBin = *((uint32*)((void*)&x));
					signBitSin = xBin;
					xBin = xBin & inv_sign_mask;
					x = *((float*)((void*)&xBin));
					signBitSin = signBitSin & sign_mask;

					/* scale by 4/Pi */
					y = x * cephes_FOPI;
					
					/* store the integer part of y in mm0 */
					emm2 = int32( y );
					/* j=(j+1) & (~1) (see the cephes sources) */
					emm2 += 1;
					emm2 = emm2 & ((uint32)~1);
					y = float( emm2 );

					emm4 = emm2;

					/* get the swap sign flag */
					emm0 = emm2 & ((uint32) 4);
					emm0 <<= 29;
					uint32 swapSignBitSin = emm0;
					
					/* get the polynom selection mask */
					emm2 = emm2 & ((uint32) 2);
					emm2 = emm2 == 0 ? 0xffffffff : 0x0;
					
					uint32	polyMask = emm2;

					/* The magic pass: "Extended precision modular arithmetic" 
					x = ((x - y * DP1) - y * DP2) - y * DP3; */
					xmm1 = minus_cephes_DP1;
					xmm2 = minus_cephes_DP2;
					xmm3 = minus_cephes_DP3;
					xmm1 = y * xmm1;
					xmm2 = y * xmm2;
					xmm3 = y * xmm3;
					x += xmm1;
					x += xmm2;
					x += xmm3;

					emm4 = emm4 - 2;
					emm4 = (~emm4) & ((uint32) 4);
					emm4 <<= 29;
					uint32	signBitCos = emm4;

					signBitSin = signBitSin ^ swapSignBitSin;

					/* Evaluate the first polynom  (0 <= x <= Pi/4) */
					float	z = x * x;
					y = coscof_p0;

					y *= z;
					y += coscof_p1;
					y *= z;
					y += coscof_p2;
					y *= z;
					y *= z;
					float	tmp = z * 0.5f;
					y -= tmp;
					y += 1.0f;

					/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

					float	y2 = sincof_p0;
					y2 *= z;
					y2 += sincof_p1;
					y2 *= z;
					y2 += sincof_p2;
					y2 *= z;
					y2 *= x;
					y2 += x;

					/* select the correct result from the two polynoms */  
					uint32		tmpMask = polyMask;
					uint32		ysin2Bin = tmpMask & *((uint32*)((void*)&y2));
					float	ysin2 = *((float*)((void*)&ysin2Bin));
					uint32		ysin1Bin = (~tmpMask) & *((uint32*)((void*)&y));
					float	ysin1 = *((float*)((void*)&ysin1Bin));
					y2 -= ysin2;
					y -= ysin1;

					xmm1 = ysin1 + ysin2;
					xmm2 = y + y2;

					/* update the sign */
					uint32	xmm1Bin = signBitSin ^ *((uint32*)((void*)&xmm1));
					sin = *((float*)((void*)&xmm1Bin));
					uint32	xmm2Bin = signBitCos ^ *((uint32*)((void*)&xmm2));
					cos = *((float*)((void*)&xmm2Bin));
				}

			};

			template<>
			class Cephes< double >
			{

			public:

				static inline
				double
				exp( double v )
				{
					double exp_hi = 88.3762626647949;
					double exp_lo = -88.3762626647949;
					
					double cephes_log2ef = 1.44269504088896341;
					double cephes_exp_C1 = 0.693359375;
					double cephes_exp_C2 = -2.12194440e-4;
					
					double cephes_exp_p0 = 1.9875691500E-4;
					double cephes_exp_p1 = 1.3981999507E-3;
					double cephes_exp_p2 = 8.3334519073E-3;
					double cephes_exp_p3 = 4.1665795894E-2;
					double cephes_exp_p4 = 1.6666665459E-1;
					double cephes_exp_p5 = 5.0000001201E-1;

					int64 i1023 = 0x3ff;
					
					double	x = v;
					double	tmp = 0.0;
					double	fx;
					int64	emm0i;
					double	one = 1.0;

					x = x > exp_hi ? exp_hi : x;
					x = x < exp_lo ? exp_lo : x;

					/* express exp(x) as exp(g + n*log(2)) */
					fx = x * cephes_log2ef;
					fx += 0.5;

					fx = floor( fx );

					tmp = fx * cephes_exp_C1;
					double	z = fx * cephes_exp_C2;
					x -= tmp;
					x -= z;

					z = x * x;

					double	y = cephes_exp_p0;
					y *= x;
					y += cephes_exp_p1;
					y *= x;
					y += cephes_exp_p2;
					y *= x;
					y += cephes_exp_p3;
					y *= x;
					y += cephes_exp_p4;
					y *= x;
					y += cephes_exp_p5;
					y *= z;
					y += x;
					y += 1.0;

					/* build 2^n */
					emm0i = int64( fx );
					emm0i += i1023;
					emm0i <<= 52;
					double pow2n = *((double*)((void*)&emm0i));
					y *= pow2n;

					return y;
				}

				static inline
				double
				cos( double v )
				{
					uint64 inv_sign_mask = ~0x8000000000000000;
					
					double minus_cephes_DP1 = -0.78515625;
					double minus_cephes_DP2 = -2.4187564849853515625e-4;
					double minus_cephes_DP3 = -3.77489497744594108e-8;
					double sincof_p0 = -1.9515295891E-4;
					double sincof_p1 = 8.3321608736E-3;
					double sincof_p2 = -1.6666654611E-1;
					double coscof_p0 = 2.443315711809948E-005;
					double coscof_p1 = -1.388731625493765E-003;
					double coscof_p2 = 4.166664568298827E-002;
					double cephes_FOPI = 1.27323954473516;
					
					double	x = v;
					double	xmm1;
					double	xmm2 = 0.0;
					double	xmm3;
					double	y;
					int64	emm0;
					int64	emm2;

					/* take the absolute value */
					uint64	xBin = *((uint64*)((void*)&x));
					xBin = xBin & inv_sign_mask;
					x = *((double*)((void*)&xBin));

					/* scale by 4/Pi */
					y = x * cephes_FOPI;
					
					/* store the integer part of y in mm0 */
					emm2 = int64( y );
					/* j=(j+1) & (~1) (see the cephes sources) */
					emm2 += 1;
					emm2 = emm2 & ((uint64)~1);
					y = double( emm2 );
					emm2 -= 2;

					/* get the swap sign flag */
					emm0 = (~emm2) & ((uint64) 4);
					emm0 <<= 61;
					
					/* get the polynom selection mask */
					emm2 = emm2 & ((uint64) 2);
					emm2 = emm2 == 0 ? 0xffffffffffffffff : 0x0;
					
					uint64	signBit = emm0;
					uint64	polyMask = emm2;

					/* The magic pass: "Extended precision modular arithmetic" 
					x = ((x - y * DP1) - y * DP2) - y * DP3; */
					xmm1 = minus_cephes_DP1;
					xmm2 = minus_cephes_DP2;
					xmm3 = minus_cephes_DP3;
					xmm1 = y * xmm1;
					xmm2 = y * xmm2;
					xmm3 = y * xmm3;
					x += xmm1;
					x += xmm2;
					x += xmm3;

					/* Evaluate the first polynom  (0 <= x <= Pi/4) */
					double	z = x * x;
					y = coscof_p0;

					y *= z;
					y += coscof_p1;
					y *= z;
					y += coscof_p2;
					y *= z;
					y *= z;
					double	tmp = z * 0.5;
					y -= tmp;
					y += 1.0;

					/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

					double	y2 = sincof_p0;
					y2 *= z;
					y2 += sincof_p1;
					y2 *= z;
					y2 += sincof_p2;
					y2 *= z;
					y2 *= x;
					y2 += x;

					/* select the correct result from the two polynoms */  
					uint64 tmpMask = polyMask;
					uint64 y2Bin = tmpMask & *((uint64*)((void*)&y2));
					y2 = *((double*)((void*)&y2Bin));
					uint64 yBin = (~tmpMask) & *((uint64*)((void*)&y));
					y = *((double*)((void*)&yBin));
					y += y2;
					yBin = *((uint64*)((void*)&y)) ^ signBit;
					y = *((double*)((void*)&yBin));
					
					return y;
				}
				
				static inline
				void
				sinCos( double v, double& sin, double& cos )
				{
					uint64 sign_mask = 0x8000000000000000;
					uint64 inv_sign_mask = ~0x8000000000000000;
					
					double minus_cephes_DP1 = -0.78515625;
					double minus_cephes_DP2 = -2.4187564849853515625e-4;
					double minus_cephes_DP3 = -3.77489497744594108e-8;
					double sincof_p0 = -1.9515295891E-4;
					double sincof_p1 = 8.3641608736E-3;
					double sincof_p2 = -1.6666654611E-1;
					double coscof_p0 = 2.443315711809948E-005;
					double coscof_p1 = -1.388731625493765E-003;
					double coscof_p2 = 4.166664568298827E-002;
					double cephes_FOPI = 1.27643954473516;
					
					double	x = v;
					double	xmm1;
					double	xmm2 = 0.0;
					double	xmm3;
					uint64	signBitSin;
					double	y;
					int64	emm0;
					int64	emm2;
					int64	emm4;

					/* take the absolute value */
					uint64	xBin = *((uint64*)((void*)&x));
					signBitSin = xBin;
					xBin = xBin & inv_sign_mask;
					x = *((double*)((void*)&xBin));
					signBitSin = signBitSin & sign_mask;

					/* scale by 4/Pi */
					y = x * cephes_FOPI;
					
					/* store the integer part of y in mm0 */
					emm2 = int64( y );
					/* j=(j+1) & (~1) (see the cephes sources) */
					emm2 += 1;
					emm2 = emm2 & ((uint64)~1);
					y = double( emm2 );

					emm4 = emm2;

					/* get the swap sign flag */
					emm0 = emm2 & ((uint64) 4);
					emm0 <<= 61;
					uint64 swapSignBitSin = emm0;
					
					/* get the polynom selection mask */
					emm2 = emm2 & ((uint64) 2);
					emm2 = emm2 == 0 ? 0xffffffffffffffff : 0x0;
					
					uint64	polyMask = emm2;

					/* The magic pass: "Extended precision modular arithmetic" 
					x = ((x - y * DP1) - y * DP2) - y * DP3; */
					xmm1 = minus_cephes_DP1;
					xmm2 = minus_cephes_DP2;
					xmm3 = minus_cephes_DP3;
					xmm1 = y * xmm1;
					xmm2 = y * xmm2;
					xmm3 = y * xmm3;
					x += xmm1;
					x += xmm2;
					x += xmm3;

					emm4 = emm4 - 2;
					emm4 = (~emm4) & ((uint64) 4);
					emm4 <<= 61;
					uint64	signBitCos = emm4;

					signBitSin = signBitSin ^ swapSignBitSin;

					/* Evaluate the first polynom  (0 <= x <= Pi/4) */
					double	z = x * x;
					y = coscof_p0;

					y *= z;
					y += coscof_p1;
					y *= z;
					y += coscof_p2;
					y *= z;
					y *= z;
					double	tmp = z * 0.5;
					y -= tmp;
					y += 1.0;

					/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

					double	y2 = sincof_p0;
					y2 *= z;
					y2 += sincof_p1;
					y2 *= z;
					y2 += sincof_p2;
					y2 *= z;
					y2 *= x;
					y2 += x;

					/* select the correct result from the two polynoms */  
					uint64		tmpMask = polyMask;
					uint64		ysin2Bin = tmpMask & *((uint64*)((void*)&y2));
					double	ysin2 = *((double*)((void*)&ysin2Bin));
					uint64		ysin1Bin = (~tmpMask) & *((uint64*)((void*)&y));
					double	ysin1 = *((double*)((void*)&ysin1Bin));
					y2 -= ysin2;
					y -= ysin1;

					xmm1 = ysin1 + ysin2;
					xmm2 = y + y2;

					/* update the sign */
					uint64	xmm1Bin = signBitSin ^ *((uint64*)((void*)&xmm1));
					sin = *((double*)((void*)&xmm1Bin));
					uint64	xmm2Bin = signBitCos ^ *((uint64*)((void*)&xmm2));
					cos = *((double*)((void*)&xmm2Bin));
				}

			};

		}



		template< typename ValueType >
		class MathBase
		{

		public:
		
			typedef ValueType	ScalarF;
			typedef int32		ScalarI;
			typedef uint32		ScalarUI;



		public:
		
			// Constants
			static inline
			ScalarF
			Pi()
			{
				return ScalarF( 3.14159265358979323846 );
			}


			// Misc operations
			static inline
			ScalarF
			floor( ScalarF x )
			{
				return ( x >= ScalarF( 0.0 ) ? ScalarF( int( x ) ): ScalarF( int( x - 1 ) ) );
			}

			static inline
			ValueType
			MakeInt32Range( ValueType n )
			{
				if( n >= ScalarF( 1073741824.0 ) )
				{
					return (ScalarF( 2.0 ) * std::fmod( n, ScalarF( 1073741824.0 ) )) - ScalarF( 1073741824.0 );
				}
				else if( n <= ValueType( -1073741824.0 ) )
				{
					return (ScalarF( 2.0 ) * std::fmod( n, ScalarF( 1073741824.0 ) )) + ScalarF( 1073741824.0 );
				}
				else
				{
					return n;
				}
			}

			static inline
			ValueType
			clamp( ValueType value, ValueType lowerBound, ValueType upperBound )
			{
				if( value < lowerBound )
				{
					return lowerBound;
				}
				else if( value > upperBound )
				{
					return upperBound;
				}
				else
				{
					return value;
				}
			}


			// Cephes
			static inline
			ValueType
			exp( ValueType v )
			{
				#if defined( LIBNOISE2_USE_CEPHES )
				return impl::Cephes< ValueType >::exp( v );
				#else
				return std::exp( v );
				#endif
			}

			static inline
			ValueType
			cos( ValueType v )
			{
				#if defined( LIBNOISE2_USE_CEPHES )
				return impl::Cephes< ValueType >::cos( v );
				#else
				return std::cos( v );
				#endif
			}
			
			static inline
			void
			sinCos( ValueType v, ValueType& sin, ValueType& cos )
			{
				#if defined( LIBNOISE2_USE_CEPHES )
				impl::Cephes< ValueType >::sinCos( v, sin, cos );
				#else
				sin = std::sin( v );
				cos = std::cos( v );
				#endif
			}


			// Order
			template< uint8 _1, uint8 _2, uint8 _3, uint8 _4 >
			struct Order
			{
				enum
				{
					o1 = _1,
					o2 = _2,
					o3 = _3,
					o4 = _4
				};
			};
	
		};

	}

}
