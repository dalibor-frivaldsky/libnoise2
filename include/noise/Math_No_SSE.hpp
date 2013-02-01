#pragma once


// Std C++
#include <cmath>


// LibNoise2
#include "noise/MathBase.hpp"




namespace noise
{

	template< typename ValueType >
	class Math: public MathBase< ValueType >
	{

		public:
		
		typedef float		ScalarF;
		typedef int			ScalarI;



		public:
		
		static inline
		ValueType
		MakeInt32Range( ValueType n )
		{
			if( n >= ValueType( 1073741824.0 ) )
			{
				return (ValueType( 2.0 ) * fmod( n, ValueType( 1073741824.0 ) )) - ValueType( 1073741824.0 );
			}
			else if( n <= ValueType( -1073741824.0 ) )
			{
				return (ValueType( 2.0 ) * fmod( n, ValueType( 1073741824.0 ) )) + ValueType( 1073741824.0 );
			}
			else
			{
				return n;
			}
		}

	};
}