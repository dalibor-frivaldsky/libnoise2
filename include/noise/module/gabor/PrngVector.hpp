#pragma once


#include <climits>
#include <cmath>


#include <noise/debug/Debug.hpp>
#include <noise/math/Math.hpp>




namespace noise
{

	namespace module
	{

		namespace gabor
		{

			template< typename ValueType >
			class PrngVector
			{

			private:

				typedef math::Math< ValueType >		M;



			private:


				typename M::Vector4I		x;



			public:

				inline
				PrngVector( const typename M::Vector4I& seed ):
				  x( seed )
				{
				}

				inline
				typename M::Vector4I
				getSeed()
				{
					return x;
				}

				inline
				typename M::Vector4I
				operator()()
				{
					LIBNOISE2_SUPPORT_CONST_ARRAY( uint32, primeA, 3039177861u );
					typename M::Vector4I	primeV = M::loadFromMemory( primeA );

					x = M::multiply( x, primeV );
					return x;
				}

				inline
				typename M::Vector4F
				uniformNormalized()
				{
					LIBNOISE2_SUPPORT_CONST_ARRAY( ValueType, uintMaxA, ValueType( UINT_MAX >> 1 ) );
					
					// TODO mention in the thesis, shifted by one to right to overcome unit->float limitation, result practically the same
					typename M::Vector4I	shiftedV = M::shiftRightLogical( (*this)(), 1 );
					typename M::Vector4F	nextRandV = M::intToFloat( shiftedV );
					typename M::Vector4F	uintMaxV = M::loadFromMemory( uintMaxA );
					return M::divide( nextRandV, uintMaxV );
				}

				inline
				typename M::Vector4F
				uniformRangeMinusOneToOne()
				{
					typename M::Vector4F	minusOneV = M::constMinusOneF();
					typename M::Vector4F	twoV = M::constTwoF();

					typename M::Vector4F	normalV = uniformNormalized();
					return M::add( minusOneV, M::multiply( normalV, twoV ) );
				}

				inline
				typename M::Vector4F
				uniformRange( const ValueType& min, const ValueType& max )
				{
					typename M::Vector4F	minV = M::vectorizeOne( min );
					typename M::Vector4F	maxV = M::vectorizeOne( max );
					typename M::Vector4F	normalV = uniformNormalized();

					return M::add( minV, M::multiply( normalV, M::subtract( maxV, minV ) ) );
				}

				// TODO mention in the thesis
				inline
				typename M::Vector4I
				poisson( const typename M::Vector4F& meanV )
				{
					typename M::Vector4I	oneIV = M::constOneI();
					typename M::Vector4F	gV = M::exp( M::multiply( meanV, M::constMinusOneF() ) );
					typename M::Vector4I	emV = M::constZeroI();
					typename M::Vector4F	tV = uniformNormalized();

					typename M::Vector4I	seedBackupV = x;
					typename M::Vector4I	tgMaskV = M::castToInt( M::greaterThan( tV, gV ) );

					while( M::isAllZeros( tgMaskV ) == false )
					{
						typename M::Vector4I	incrementV = M::bitAnd( oneIV, tgMaskV );
						emV = M::add( emV, incrementV );

						typename M::Vector4F	tMulV = uniformNormalized();
						tV = M::multiply( tV, tMulV );

						x = M::blend( seedBackupV, x, tgMaskV );

						seedBackupV = x;
						tgMaskV = M::castToInt( M::greaterThan( tV, gV ) );
					}

					return emV;
				}

			};

		}

	}

}
