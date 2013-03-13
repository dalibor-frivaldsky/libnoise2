#pragma once


// libnoise
#include <noise/math/Math.hpp>




namespace noise
{

	namespace math
	{

		template< typename ValueType >
		class Interpolations
		{

			public:
			
			typedef Math< ValueType >		M;
			typedef typename M::Vector4F	Vector4F;



			public:
			
			/// Performs cubic interpolation between two values bound between two other
			/// values.
			///
			/// @param n0 The value before the first value.
			/// @param n1 The first value.
			/// @param n2 The second value.
			/// @param n3 The value after the second value.
			/// @param a The alpha value.
			///
			/// @returns The interpolated value.
			///
			/// The alpha value should range from 0.0 to 1.0.  If the alpha value is
			/// 0.0, this function returns @a n1.  If the alpha value is 1.0, this
			/// function returns @a n2.
			static inline
			ValueType 
			CubicInterp( ValueType n0, ValueType n1, ValueType n2, ValueType n3, ValueType a )
			{
				ValueType	p = (n3 - n2) - (n0 - n1);
				ValueType	q = (n0 - n1) - p;
				ValueType	r = n2 - n0;
				ValueType	s = n1;
				
				return p * a * a * a + q * a * a + r * a + s;
			}

			/// Performs linear interpolation between two values.
			///
			/// @param n0 The first value.
			/// @param n1 The second value.
			/// @param a The alpha value.
			///
			/// @returns The interpolated value.
			///
			/// The alpha value should range from 0.0 to 1.0.  If the alpha value is
			/// 0.0, this function returns @a n0.  If the alpha value is 1.0, this
			/// function returns @a n1.
			static inline 
			ValueType
			LinearInterp( ValueType n0, ValueType n1, ValueType a )
			{
				return ((ValueType(1.0) - a) * n0) + (a * n1);
			}
			
			static inline
			Vector4F
			LinearInterpV( Vector4F& n0V, Vector4F& n1V, Vector4F& aV )
			{
				Vector4F		l = M::vectorizeOne( ValueType( 1.0 ) );
				Vector4F		r;
				
				l = M::subtract( l, aV );
				l = M::multiply( l, n0V );
				
				r = M::multiply( aV, n1V );
				
				return M::add( l, r );
			}

			/// Maps a value onto a cubic S-curve.
			///
			/// @param a The value to map onto a cubic S-curve.
			///
			/// @returns The mapped value.
			///
			/// @a a should range from 0.0 to 1.0.
			///
			/// The derivitive of a cubic S-curve is zero at @a a = 0.0 and @a a =
			/// 1.0
			static inline 
			ValueType
			SCurve3( ValueType a )
			{
				return (a * a * (ValueType( 3.0 ) - ValueType( 2.0 ) * a));
			}
			
			static inline
			Vector4F
			SCurve3V( Vector4F aV )
			{
				Vector4F	tmpV;
				Vector4F	constantsV = M::vectorizeOne( ValueType( 2.0 ) );
				
				tmpV = M::multiply( constantsV, aV );
				
				constantsV = M::vectorizeOne( ValueType( 3.0 ) );
				tmpV = M::subtract( constantsV, tmpV );
				
				tmpV = M::multiply( aV, tmpV );
				tmpV = M::multiply( aV, tmpV );
				
				return tmpV;
			}

			/// Maps a value onto a quintic S-curve.
			///
			/// @param a The value to map onto a quintic S-curve.
			///
			/// @returns The mapped value.
			///
			/// @a a should range from 0.0 to 1.0.
			///
			/// The first derivitive of a quintic S-curve is zero at @a a = 0.0 and
			/// @a a = 1.0
			///
			/// The second derivitive of a quintic S-curve is zero at @a a = 0.0 and
			/// @a a = 1.0
			static inline 
			ValueType
			SCurve5( ValueType a )
			{
				ValueType	a3 = a * a * a;
				ValueType	a4 = a3 * a;
				ValueType	a5 = a4 * a;
				
				return (ValueType( 6.0 ) * a5) - (ValueType( 15.0 ) * a4) + (ValueType( 10.0 ) * a3);
			}
			
			static inline
			Vector4F
			SCurve5V( Vector4F aV )
			{
				Vector4F	tmpV;
				Vector4F	constantsV;
				Vector4F	a3, a4, a5;
				Vector4F	resultV;
				
				a3 = M::multiply( aV, aV );
				a3 = M::multiply( a3, aV );
				
				a4 = M::multiply( a3, aV );
				
				a5 = M::multiply( a4, aV );
				
				constantsV = M::vectorizeOne( ValueType( 6.0 ) );
				resultV = M::multiply( a5, constantsV );
				
				constantsV = M::vectorizeOne( ValueType( 15.0 ) );
				tmpV = M::multiply( a4, constantsV );
				resultV = M::subtract( resultV, tmpV );
				
				constantsV = M::vectorizeOne( ValueType( 10.0 ) );
				tmpV = M::multiply( a3, constantsV );
				
				return M::add( resultV, tmpV );
			}
			
		};

	}

}
