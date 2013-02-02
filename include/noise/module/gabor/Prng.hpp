#pragma once


#include <climits>
#include <cmath>




namespace noise
{

	namespace module
	{

		namespace gabor
		{

			template< typename ValueType >
			class Prng
			{

			private:

				unsigned int	x;



			public:

				Prng( unsigned int seed ):
				  x( seed )
				{
				}

				void
				seed( unsigned int seed )
				{
					x = seed;
				}

				unsigned int
				getSeed()
				{
					return x;
				}

				unsigned
				operator()()
				{
					x *= 3039177861u;
					return x;
				}

				ValueType
				uniformNormalized()
				{
					return ValueType( (*this)() ) / ValueType( UINT_MAX );
				}

				ValueType
				uniformRangeMinusOneToOne()
				{
					return ValueType( -1.0 ) + (uniformNormalized() * ValueType( 2.0 ));
				}

				ValueType
				uniformRange( ValueType min, ValueType max )
				{
					return min + (uniformNormalized() * (max - min));
				}

				unsigned int
				poisson( ValueType mean )
				{
					ValueType		g = std::exp( -mean );
					unsigned int	em = 0;
					ValueType		t = uniformNormalized();

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
