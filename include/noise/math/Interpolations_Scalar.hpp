#pragma once




namespace noise
{

	namespace math
	{

		template< typename ValueType >
		class Interpolations
		{

			public:
			
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

			static inline 
			ValueType
			LinearInterp( ValueType n0, ValueType n1, ValueType a )
			{
				return ((ValueType(1.0) - a) * n0) + (a * n1);
			}
			
			static inline 
			ValueType
			SCurve3( ValueType a )
			{
				return (a * a * (ValueType( 3.0 ) - ValueType( 2.0 ) * a));
			}
			
			static inline 
			ValueType
			SCurve5( ValueType a )
			{
				ValueType	a3 = a * a * a;
				ValueType	a4 = a3 * a;
				ValueType	a5 = a4 * a;
				
				return (ValueType( 6.0 ) * a5) - (ValueType( 15.0 ) * a4) + (ValueType( 10.0 ) * a3);
			}
			
		};

	}

}
