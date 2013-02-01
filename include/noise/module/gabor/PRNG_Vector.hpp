#pragma once


#include <climits>
#include <cmath>


#include <noise/Math.hpp>




namespace noise
{

	namespace module
	{

		namespace gabor
		{

			template< typename ValueType >
			class PRNG_Vector
			{

			public:

				typedef Math< ValueType >		M;



			private:

				typename M::Vector4I		x;



			public:

				PRNG_Vector( const typename M::Vector4I& seed ):
				  x( seed )
				{
				}

				typename M::Vector4I
				getSeed()
				{
					return x;
				}

				typename M::Vector4I
				operator()()
				{
					static VECTOR_ALIGN( unsigned int	primeA[ 4 ] ) = { 3039177861u, 3039177861u, 3039177861u, 3039177861u };
					typename M::Vector4I	primeV = M::loadFromMemory( (typename M::ScalarI*) primeA );

					x = M::multiply( x, primeV );
					return x;
				}

				typename M::Vector4I
				uniformNormalized()
				{
					static VECTOR_ALIGN( ValueType	uintMaxA[ 4 ] ) = { ValueType( UINT_MAX >> 1 ), ValueType( UINT_MAX >> 1 ), ValueType( UINT_MAX >> 1 ), ValueType( UINT_MAX >> 1 ) };

					// TODO mention in the thesis, shifted by one to right to overcome unit->float limitation, result practically the same
					typename M::Vector4I	shiftedV = _mm_srli_epi32( (*this)(), 1 );
					typename M::Vector4F	nextRandV = M::intToFloat( shiftedV );
					typename M::Vector4F	uintMaxV = M::loadFromMemory( uintMaxA );
					return M::divide( nextRandV, uintMaxV );
				}

				typename M::Vector4I
				uniformRangeMinusOneToOne()
				{
					static VECTOR_ALIGN( ValueType	minusOneA[ 4 ] ) = { ValueType( -1.0 ), ValueType( -1.0 ), ValueType( -1.0 ), ValueType( -1.0 ) };
					static VECTOR_ALIGN( ValueType	twoA[ 4 ] ) = { ValueType( 2.0 ), ValueType( 2.0 ), ValueType( 2.0 ), ValueType( 2.0 ) };

					typename M::Vector4F	minusOneV = M::loadFromMemory( minusOneA );
					typename M::Vector4F	twoV = M::loadFromMemory( twoA );

					typename M::Vector4F	normalV = uniformNormalized();
					return M::add( minusOneV, M::multiply( normalV, twoV ) );
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
