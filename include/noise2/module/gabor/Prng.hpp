#pragma once


#include <climits>
#include <cmath>


#include <noise2/BasicTypes.hpp>




namespace noise2
{

	namespace module
	{

		namespace gabor
		{

			template< typename ValueType >
			class Prng
			{

			private:

				uint32	x;



			public:

				inline
				Prng( uint32 seed ):
				  x( seed )
				{
				}

				inline
				void
				seed( uint32 seed )
				{
					x = seed;
				}

				inline
				uint32
				getSeed()
				{
					return x;
				}

				inline
				uint32
				operator()()
				{
					x *= 3039177861u;
					return x;
				}

				inline
				ValueType
				uniformNormalized()
				{
					return ValueType( (*this)() ) / ValueType( UINT_MAX );
				}

				inline
				ValueType
				uniformRangeMinusOneToOne()
				{
					return ValueType( -1.0 ) + (uniformNormalized() * ValueType( 2.0 ));
				}

				inline
				ValueType
				uniformRange( ValueType min, ValueType max )
				{
					return min + (uniformNormalized() * (max - min));
				}

				inline
				uint32
				poisson( ValueType mean )
				{
					ValueType	g = std::exp( -mean );
					uint32		em = 0;
					ValueType	t = uniformNormalized();

					while( t > g )
					{
						++em;
						t *= uniformNormalized();
					}

					return em;
				}

			};

		}

	}

}
