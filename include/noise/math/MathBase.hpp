#pragma once


#include <cmath>


#include <noise/BasicTypes.hpp>




namespace noise
{

	namespace math
	{

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
				return ( x > ScalarF( 0.0 ) ? ScalarF( int( x ) ): ScalarF( int( x - 1 ) ) );
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
