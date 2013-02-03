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
			class PrngVector
			{

			public:

				typedef Math< ValueType >		M;



			private:

				typename M::Vector4I		x;



			public:

				PrngVector( const typename M::Vector4I& seed ):
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
					static VECTOR4_ALIGN( unsigned int	primeA[ 4 ] ) = { 3039177861u, 3039177861u, 3039177861u, 3039177861u };
					typename M::Vector4I	primeV = M::loadFromMemory( (typename M::ScalarI*) primeA );

					x = M::multiply( x, primeV );
					return x;
				}

				typename M::Vector4F
				uniformNormalized()
				{
					static VECTOR4_ALIGN( ValueType	uintMaxA[ 4 ] ) = { ValueType( UINT_MAX >> 1 ), ValueType( UINT_MAX >> 1 ), ValueType( UINT_MAX >> 1 ), ValueType( UINT_MAX >> 1 ) };

					// TODO mention in the thesis, shifted by one to right to overcome unit->float limitation, result practically the same
					typename M::Vector4I	shiftedV = _mm_srli_epi32( (*this)(), 1 );
					typename M::Vector4F	nextRandV = M::intToFloat( shiftedV );
					typename M::Vector4F	uintMaxV = M::loadFromMemory( uintMaxA );
					return M::divide( nextRandV, uintMaxV );
				}

				typename M::Vector4F
				uniformRangeMinusOneToOne()
				{
					static VECTOR4_ALIGN( ValueType	minusOneA[ 4 ] ) = { ValueType( -1.0 ), ValueType( -1.0 ), ValueType( -1.0 ), ValueType( -1.0 ) };
					static VECTOR4_ALIGN( ValueType	twoA[ 4 ] ) = { ValueType( 2.0 ), ValueType( 2.0 ), ValueType( 2.0 ), ValueType( 2.0 ) };

					typename M::Vector4F	minusOneV = M::loadFromMemory( minusOneA );
					typename M::Vector4F	twoV = M::loadFromMemory( twoA );

					typename M::Vector4F	normalV = uniformNormalized();
					return M::add( minusOneV, M::multiply( normalV, twoV ) );
				}

				typename M::Vector4F
				uniformRange( const ValueType& min, const ValueType& max )
				{
					typename M::Vector4F	minV = M::vectorizeOne( min );
					typename M::Vector4F	maxV = M::vectorizeOne( max );
					typename M::Vector4F	normalV = uniformNormalized();

					return M::add( minV, M::multiply( normalV, M::subtract( maxV, minV ) ) );
				}

				// TODO mention in the thesis
				typename M::Vector4I
				poisson( const typename M::Vector4F& meanV )
				{
					static VECTOR4_ALIGN( ValueType		minusOneFA[ 4 ] ) = { -1.0, -1.0, -1.0, -1.0 };
					static VECTOR4_ALIGN( unsigned int	oneIA[ 4 ] ) = { 1, 1, 1, 1 };
					static VECTOR4_ALIGN( unsigned int	negMaskA[ 4 ] ) = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };

					typename M::Vector4I	oneIV = M::loadFromMemory( (typename M::ScalarI*) oneIA );
					typename M::Vector4I	negMaskV = M::loadFromMemory( (typename M::ScalarI*) negMaskA );
					typename M::Vector4F	gV = M::exp( M::multiply( meanV, M::loadFromMemory( minusOneFA ) ) );
					typename M::Vector4I	emV = _mm_setzero_si128();
					typename M::Vector4F	tV = uniformNormalized();

					typename M::Vector4I	seedBackupV = x;
					typename M::Vector4I	tgMaskV = _mm_castps_si128( _mm_cmpgt_ps( tV, gV ) );

					while( !M::isAllZeros( tgMaskV ) )
					{
						typename M::Vector4I	incrementV = _mm_and_si128( oneIV, tgMaskV );
						emV = M::add( emV, incrementV );

						typename M::Vector4F	tMulV = uniformNormalized();
						tV = M::multiply( tV, tMulV );

						typename M::Vector4I	seedNextPartV = _mm_and_si128( x, tgMaskV );
						typename M::Vector4I	negTgMaskV = _mm_xor_si128( tgMaskV, negMaskV );
						typename M::Vector4I	seedBackupPartV = _mm_and_si128( seedBackupV, negTgMaskV );
						x = M::add( seedNextPartV, seedBackupPartV );

						seedBackupV = x;
						tgMaskV = _mm_castps_si128( _mm_cmpgt_ps( tV, gV ) );
					}

					return emV;
				}

			};

		}

	}

}
