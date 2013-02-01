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
			class PRNG
			{

			private:

				unsigned int	x;



			public:

				PRNG():
				  x( 0 )
				{
				}

				PRNG( unsigned int seed ):
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

				unsigned int
				poissonG( ValueType g )
				{
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
