#pragma once


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

}
