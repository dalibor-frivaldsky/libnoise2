#pragma once


#include <climits>
#include <cmath>
#include <iostream>


#include <noise/math/Math.hpp>
#include <noise/module/Module.hpp>
#include <noise/module/gabor/GaborBase.hpp>
#include <noise/module/gabor/Prng.hpp>
#include <noise/module/gabor/PrngVector.hpp>

#include <noise/debug/Debug.hpp>




namespace noise
{

	namespace module
	{

		template< typename ValueT >
		class Gabor< ValueT, 2 >: public Module< ValueT, 2 >, public gabor::GaborBase< ValueT >
		{

		public:

			typedef ValueT								ValueType;
			static const unsigned						Dimension = 2;
			typedef Module< ValueType, Dimension >		ModuleType;
			typedef gabor::GaborBase< ValueType >		BaseType;
			typedef Gabor< ValueType, Dimension >		ThisType;



		private:
			typedef gabor::Prng< ValueType >			PrngType;
			typedef gabor::PrngVector< ValueType >		PrngVectorType;
			typedef math::Math< ValueType >				M;



		public:

			Gabor():
			  ModuleType( 0 ),
			  BaseType()
			{}

			virtual
			~Gabor()
			{}

			virtual
			ValueType
			GetValue( ValueType x, ValueType y ) const
			{
				x /= this->GetKernelRadius();
				y /= this->GetKernelRadius();

				ValueType	intX = M::floor( x );
				ValueType	intY = M::floor( y );
				ValueType	fracX = x - intX;
				ValueType	fracY = y - intY;
				int			i = int( intX );
				int			j = int( intY );
				ValueType	noise = ValueType( 0.0 );
				typename M::Vector4F	fracXV = M::vectorizeOne( x - intX );
				typename M::Vector4F	fracYV = M::vectorizeOne( y - intY );

				//static VECTOR4_ALIGN( int		diA[ 9 ] ) = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
				//static VECTOR4_ALIGN( int		djA[ 9 ] ) = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
				static VECTOR4_ALIGN( int		diA[ 9 ] ) = { -1, -1, 1, 1, -1, 0, 0, 1, 0 };
				static VECTOR4_ALIGN( int		djA[ 9 ] ) = { -1, 1, -1, 1, 0, -1, 1, 0, 0 };

				typename M::Vector4I		iV = M::vectorizeOne( i );
				typename M::Vector4I		jV = M::vectorizeOne( j );

				for( int m = 0; m < 2; ++m )
				{
					typename M::Vector4I	diV = M::loadFromMemory( diA + (m * 4) );
					typename M::Vector4I	djV = M::loadFromMemory( djA + (m * 4) );
					typename M::Vector4I	diiV = M::add( diV, iV );
					typename M::Vector4I	djjV = M::add( djV, jV );
					typename M::Vector4F	xV = M::subtract( fracXV, M::intToFloat( diV ) );
					typename M::Vector4F	yV = M::subtract( fracYV, M::intToFloat( djV ) );
					typename M::Vector4I	mortonV = mortonVectorized( diiV, djjV );
					
					noise += cellVectorizedNoArrays( mortonV, xV, yV );
					//noise += cellVectorized( mortonV, xV, yV );
				}

				for( int c = 8; c < 9; ++c )
				{
					unsigned int	s = morton( diA[ c ] + i, djA[ c ] + j );
					noise += cell( s, fracX - diA[ c ], fracY - djA[ c ] );
				}

				return noise / (ValueType( 3.0 ) * std::sqrt( variance() ));
			}



		private:

			inline
			ValueType
			cell( unsigned int s, ValueType x, ValueType y ) const
			{
				ValueType		noise = ValueType( 0.0 );

				PrngType		prng( s );
				ValueType		numberOfImpulsesPerCell = this->GetImpulseDensity() * this->GetKernelRadius() * this->GetKernelRadius();
				unsigned int	numberOfImpulses = prng.poisson( numberOfImpulsesPerCell );

				int		toCalculate = 0;
				VECTOR4_ALIGN( ValueType	xInputA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
				VECTOR4_ALIGN( ValueType	yInputA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
				VECTOR4_ALIGN( ValueType	wiInputA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
				VECTOR4_ALIGN( ValueType	F0InputA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
				VECTOR4_ALIGN( ValueType	omega0InputA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };


				for( unsigned int i = 0; i < numberOfImpulses; ++i )
				{
					ValueType	xi = prng.uniformNormalized();
					ValueType	yi = prng.uniformNormalized();
					ValueType	wi = prng.uniformRangeMinusOneToOne();
					ValueType	F0 = prng.uniformRange( this->GetFrequencyRangeStart(), this->GetFrequencyRangeEnd() );
					ValueType	omega0 = prng.uniformRange( this->GetAngularRangeStart(), this->GetAngularRangeEnd() );
					ValueType	xix = x - xi;
					ValueType	yiy = y - yi;

					if( ((xix * xix) + (yiy * yiy)) < ValueType( 1.0 ) )
					{
						xInputA[ toCalculate ] = xix * this->GetKernelRadius();
						yInputA[ toCalculate ] = yiy * this->GetKernelRadius();
						wiInputA[ toCalculate ] = wi;
						F0InputA[ toCalculate ] = F0;
						omega0InputA[ toCalculate ] = omega0;
						++toCalculate;
					}

					if( toCalculate == 4 ||
						(i == numberOfImpulses - 1 && toCalculate > 0) )
					{
						typename M::Vector4F		xInputV = M::loadFromMemory( xInputA );
						typename M::Vector4F		yInputV = M::loadFromMemory( yInputA );
						typename M::Vector4F		kV = M::vectorizeOne( this->GetK() );
						typename M::Vector4F		aV = M::vectorizeOne( this->GetA() );
						typename M::Vector4F		F0V = M::loadFromMemory( F0InputA );
						typename M::Vector4F		omega0V = M::loadFromMemory( omega0InputA );
						typename M::Vector4F		wiV = M::loadFromMemory( wiInputA );

						typename M::Vector4F		gaborV = gaborVectorized( kV, aV, F0V, omega0V, xInputV, yInputV );
						gaborV = M::multiply( wiV, gaborV );

						SSE_ALIGN( ValueType	gaborResult[ 4 ] );
						M::storeToMemory( gaborResult, gaborV );

						for( int j = 0; j < toCalculate; ++j )
						{
							noise += gaborResult[ j ];
						}

						toCalculate = 0;
					}
				}

				return noise;
			}

			struct
			Compacter
			{

				typename M::Vector4F	xInputVPrev;
				typename M::Vector4F	yInputVPrev;
				typename M::Vector4F	wiVPrev;
				typename M::Vector4F	F0VPrev;
				typename M::Vector4F	omega0VPrev;
				typename M::Vector4I	calcMaskVPrev;

				inline
				Compacter():
				  xInputVPrev( _mm_setzero_si128() ),
				  yInputVPrev( _mm_setzero_si128() ),
				  wiVPrev( _mm_setzero_si128() ),
				  F0VPrev( _mm_setzero_si128() ),
				  omega0VPrev( _mm_setzero_si128() ),
				  calcMaskVPrev( _mm_setzero_si128() )
				{
				}

				inline
				bool
				compact( typename M::Vector4F& xInputVNext,
						 typename M::Vector4F& yInputVNext,
						 typename M::Vector4F& wiVNext,
						 typename M::Vector4F& F0VNext,
						 typename M::Vector4F& omega0VNext,
						 typename M::Vector4I& calcMaskVNext )
				{
					if( M::isAllZeros( calcMaskVNext ) == true )
					{
						return false;
					}

					typename M::Vector4I	compactMaskV;// = _mm_xor_si128( calcMaskVPrev, _mm_or_si128( calcMaskVPrev, calcMaskVNext ) );
					typename M::Vector4I	keepMaskV;// = _mm_and_si128( calcMaskVPrev, calcMaskVNext );
					bool	calculate;// = !M::isAllZeros( keepMaskV );

					//if( calculate == true )
					{
						//debug::printVector( xInputVPrev );

						xInputVPrev = M::shiftRightLoop( xInputVPrev, 1 );
						yInputVPrev = M::shiftRightLoop( yInputVPrev, 1 );
						wiVPrev = M::shiftRightLoop( wiVPrev, 1 );
						F0VPrev = M::shiftRightLoop( F0VPrev, 1 );
						omega0VPrev = M::shiftRightLoop( omega0VPrev, 1 );
						calcMaskVPrev = M::shiftRightLoop( calcMaskVPrev, 1 );

						//debug::printVector( xInputVPrev );

						compactMaskV = _mm_xor_si128( calcMaskVPrev, _mm_or_si128( calcMaskVPrev, calcMaskVNext ) );
						keepMaskV = _mm_and_si128( calcMaskVPrev, calcMaskVNext );
						calculate = !M::isAllZeros( keepMaskV );
					}

					xInputVNext = compactOne( compactMaskV, keepMaskV, xInputVPrev, xInputVNext, calculate );
					yInputVNext = compactOne( compactMaskV, keepMaskV, yInputVPrev, yInputVNext, calculate );
					wiVNext = compactOne( compactMaskV, keepMaskV, wiVPrev, wiVNext, calculate );
					F0VNext = compactOne( compactMaskV, keepMaskV, F0VPrev, F0VNext, calculate );
					omega0VNext = compactOne( compactMaskV, keepMaskV, omega0VPrev, omega0VNext, calculate );

					if( calculate == false )
					{
						calcMaskVPrev = _mm_or_si128( calcMaskVPrev, calcMaskVNext );
					}
					else
					{
						calcMaskVNext = _mm_or_si128( calcMaskVPrev, calcMaskVNext );
						calcMaskVPrev = keepMaskV;
					}

					return calculate;
				}



			private:

				inline
				typename M::Vector4F
				compactOne( const typename M::Vector4I& compactMaskV, const typename M::Vector4I& keepMaskV,
							typename M::Vector4F& vPrev, const typename M::Vector4F& vNext, bool calculate )
				{
					static VECTOR4_ALIGN( typename M::ScalarUI	negMaskA[ 4 ] ) = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
					typename M::Vector4I	negMaskV = M::loadFromMemory( negMaskA );

					typename M::Vector4I	compactV = _mm_and_si128( _mm_castps_si128( vNext ), compactMaskV );
					typename M::Vector4I	negCompactMaskV = _mm_xor_si128( compactMaskV, negMaskV );
					compactV = M::add( compactV, _mm_and_si128( vPrev, negCompactMaskV ) );

					typename M::Vector4I	keepV = _mm_and_si128( _mm_castps_si128( vNext ), keepMaskV );

					if( calculate == false )
					{
						vPrev = _mm_castsi128_ps( compactV );
						return _mm_castsi128_ps( keepV );
					}
					else
					{
						vPrev = _mm_castsi128_ps( keepV );
						return _mm_castsi128_ps( compactV );
					}
				}

			};

			inline
			ValueType
			cellVectorizedNoArrays( const typename M::Vector4I sV, const typename M::Vector4F xV, const typename M::Vector4F yV ) const
			{
				static int	total = 0;
				static int	calced = 0;
				Compacter	compacter;

				ValueType	noise = ValueType( 0.0 );

				PrngVectorType			prngVector( sV );
				typename M::Vector4F	impulseDensityV = M::vectorizeOne( this->GetImpulseDensity() );
				typename M::Vector4F	kernelRadiusV = M::vectorizeOne( this->GetKernelRadius() );
				typename M::Vector4F	numberOfImpulsesPerCellV = M::multiply( impulseDensityV, M::multiply( kernelRadiusV, kernelRadiusV ) );
				typename M::Vector4I	numberOfImpulsesV = prngVector.poisson( numberOfImpulsesPerCellV );

				VECTOR4_ALIGN( unsigned int	numberOfImpulsesA[ 4 ] );
				M::storeToMemory( (typename M::ScalarI*) numberOfImpulsesA, numberOfImpulsesV );

				unsigned int			maxNumberOfImpulses = findMax( numberOfImpulsesA );
				typename M::Vector4I	oneIV = M::vectorizeOne( (typename M::ScalarI) 1 );
				typename M::Vector4F	oneFV = M::vectorizeOne( ValueType( 1.0 ) );

				for( unsigned int i = 0; i < maxNumberOfImpulses; ++i )
				{
					typename M::Vector4I	impulseMaskV = _mm_cmpgt_epi32( numberOfImpulsesV, _mm_setzero_si128() );
					typename M::Vector4F	xiV = prngVector.uniformNormalized();
					typename M::Vector4F	yiV = prngVector.uniformNormalized();
					typename M::Vector4F	wiV = prngVector.uniformRangeMinusOneToOne();
					typename M::Vector4F	F0V = prngVector.uniformRange( this->GetFrequencyRangeStart(), this->GetFrequencyRangeEnd() );
					typename M::Vector4F	omega0V = prngVector.uniformRange( this->GetAngularRangeStart(), this->GetAngularRangeEnd() );
					typename M::Vector4F	xInputV = M::subtract( xV, xiV );
					typename M::Vector4F	yInputV = M::subtract( yV, yiV );

					typename M::Vector4F	radiusXV = M::multiply( xInputV, xInputV );
					typename M::Vector4F	radiusYV = M::multiply( yInputV, yInputV );
					typename M::Vector4F	radiusV = M::add( radiusXV, radiusYV );
					typename M::Vector4I	radiusMaskV = _mm_castps_si128( _mm_cmplt_ps( radiusV, oneFV ) );
					typename M::Vector4I	calcMaskV = _mm_and_si128( radiusMaskV, impulseMaskV );

					if( compacter.compact( xInputV, yInputV, wiV, F0V, omega0V, calcMaskV ) == true )
					{
						noise += cellPart( xInputV, yInputV, wiV, F0V, omega0V, calcMaskV );	
					}
					if( i == maxNumberOfImpulses - 1 )
					{
						noise += cellPart( compacter.xInputVPrev,
										   compacter.yInputVPrev,
										   compacter.wiVPrev,
										   compacter.F0VPrev,
										   compacter.omega0VPrev,
										   compacter.calcMaskVPrev );
					}

					numberOfImpulsesV = M::subtract( numberOfImpulsesV, _mm_and_si128( impulseMaskV, oneIV ) );
				}

				return noise;
			}

			inline
			ValueType
			cellPart( const typename M::Vector4F& xInputV, const typename M::Vector4F& yInputV,
					  const typename M::Vector4F& wiV, const typename M::Vector4F& F0V,
					  const typename M::Vector4F& omega0V, const typename M::Vector4I& calcMaskV ) const
			{
				ValueType	noise = ValueType( 0.0 );

				typename M::Vector4F	kernelRadiusV = M::vectorizeOne( this->GetKernelRadius() );

				typename M::Vector4F	xV = M::multiply( xInputV, kernelRadiusV );
				typename M::Vector4F	yV = M::multiply( yInputV, kernelRadiusV );
				typename M::Vector4F	kV = M::vectorizeOne( this->GetK() );
				typename M::Vector4F	aV = M::vectorizeOne( this->GetA() );

				typename M::Vector4F	gaborV = gaborVectorized( kV, aV, F0V, omega0V, xV, yV );
				gaborV = M::multiply( wiV, gaborV );

				VECTOR4_ALIGN( typename M::ScalarF		gaborA[ 4 ] );
				VECTOR4_ALIGN( typename M::ScalarUI		calcMaskA[ 4 ] );
				M::storeToMemory( gaborA, gaborV );
				M::storeToMemory( calcMaskA, calcMaskV );

				for( int j = 0; j < 4; ++j )
				{
					if( calcMaskA[ j ] == 0xffffffff )
					{
						noise += gaborA[ j ];
					}
				}
				
				return noise;
			}

			inline
			ValueType
			cellVectorized( const typename M::Vector4I sV, const typename M::Vector4F xV, const typename M::Vector4F yV ) const
			{
				ValueType	noise = ValueType( 0.0 );

				PrngVectorType			prngVector( sV );
				typename M::Vector4F	impulseDensityV = M::vectorizeOne( this->GetImpulseDensity() );
				typename M::Vector4F	kernelRadiusV = M::vectorizeOne( this->GetKernelRadius() );
				typename M::Vector4F	numberOfImpulsesPerCellV = M::multiply( impulseDensityV, M::multiply( kernelRadiusV, kernelRadiusV ) );
				typename M::Vector4I	numberOfImpulsesV = prngVector.poisson( numberOfImpulsesPerCellV );

				VECTOR4_ALIGN( unsigned int	numberOfImpulsesA[ 4 ] );
				M::storeToMemory( (typename M::ScalarI*) numberOfImpulsesA, numberOfImpulsesV );

				unsigned int			maxNumberOfImpulses = findMax( numberOfImpulsesA );
				typename M::Vector4I	oneIV = M::vectorizeOne( (typename M::ScalarI) 1 );
				typename M::Vector4F	oneFV = M::vectorizeOne( ValueType( 1.0 ) );
				
#if defined( _MSC_VER )
				ValueType*		xInputA = (ValueType*) _alloca( 4 * maxNumberOfImpulses * sizeof( ValueType ) );
				ValueType*		yInputA = (ValueType*) _alloca( 4 * maxNumberOfImpulses * sizeof( ValueType ) );
				ValueType*		wiInputA = (ValueType*) _alloca( 4 * maxNumberOfImpulses * sizeof( ValueType ) );
				ValueType*		F0InputA = (ValueType*) _alloca( 4 * maxNumberOfImpulses * sizeof( ValueType ) );
				ValueType*		omega0InputA = (ValueType*) _alloca( 4 * maxNumberOfImpulses * sizeof( ValueType ) );
				unsigned int*	calcMaskA = (unsigned int*) _alloca( 4 * maxNumberOfImpulses * sizeof( unsigned int ) );
#else
				VECTOR4_ALIGN( ValueType		xInputA[ 4 * maxNumberOfImpulses ] );
				VECTOR4_ALIGN( ValueType		yInputA[ 4 * maxNumberOfImpulses ] );
				VECTOR4_ALIGN( ValueType		wiInputA[ 4 * maxNumberOfImpulses ] );
				VECTOR4_ALIGN( ValueType		F0InputA[ 4 * maxNumberOfImpulses ] );
				VECTOR4_ALIGN( ValueType		omega0InputA[ 4 * maxNumberOfImpulses ] );
				VECTOR4_ALIGN( unsigned int		calcMaskA[ 4 * maxNumberOfImpulses ] );
#endif

				for( unsigned int i = 0; i < maxNumberOfImpulses; ++i )
				{
					typename M::Vector4I	impulseMaskV = _mm_cmpgt_epi32( numberOfImpulsesV, _mm_setzero_si128() );
					typename M::Vector4F	xiV = prngVector.uniformNormalized();
					typename M::Vector4F	yiV = prngVector.uniformNormalized();
					typename M::Vector4F	wiV = prngVector.uniformRangeMinusOneToOne();
					typename M::Vector4F	F0V = prngVector.uniformRange( this->GetFrequencyRangeStart(), this->GetFrequencyRangeEnd() );
					typename M::Vector4F	omega0V = prngVector.uniformRange( this->GetAngularRangeStart(), this->GetAngularRangeEnd() );
					typename M::Vector4F	xixV = M::subtract( xV, xiV );
					typename M::Vector4F	yiyV = M::subtract( yV, yiV );

					typename M::Vector4F	radiusXV = M::multiply( xixV, xixV );
					typename M::Vector4F	radiusYV = M::multiply( yiyV, yiyV );
					typename M::Vector4F	radiusV = M::add( radiusXV, radiusYV );
					typename M::Vector4I	radiusMaskV = _mm_castps_si128( _mm_cmplt_ps( radiusV, oneFV ) );
					typename M::Vector4I	calcMaskV = _mm_and_si128( radiusMaskV, impulseMaskV );

					M::storeToMemory( xInputA + (i * 4 ), xixV );
					M::storeToMemory( yInputA + (i * 4 ), yiyV );
					M::storeToMemory( wiInputA + (i * 4 ), wiV );
					M::storeToMemory( F0InputA + (i * 4 ), F0V );
					M::storeToMemory( omega0InputA + (i * 4 ), omega0V );
					M::storeToMemory( (typename M::ScalarI*) calcMaskA + (i * 4 ), calcMaskV );

					numberOfImpulsesV = M::subtract( numberOfImpulsesV, _mm_and_si128( impulseMaskV, oneIV ) );
				}

				int		toCalculate = 0;
				VECTOR4_ALIGN( ValueType	xInputPartA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
				VECTOR4_ALIGN( ValueType	yInputPartA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
				VECTOR4_ALIGN( ValueType	wiInputPartA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
				VECTOR4_ALIGN( ValueType	F0InputPartA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
				VECTOR4_ALIGN( ValueType	omega0InputPartA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
				for( unsigned int g = 0; g < maxNumberOfImpulses * 4; ++g )
				{
					if( calcMaskA[ g ] == 0xffffffff )
					{
						xInputPartA[ toCalculate ] = xInputA[ g ];
						yInputPartA[ toCalculate ] = yInputA[ g ];
						wiInputPartA[ toCalculate ] = wiInputA[ g ];
						F0InputPartA[ toCalculate ] = F0InputA[ g ];
						omega0InputPartA[ toCalculate ] = omega0InputA[ g ];
						++toCalculate;
					}

					if( toCalculate == 4 ||
						(g == maxNumberOfImpulses * 4 - 1 && toCalculate > 0) )
					{
						typename M::Vector4F		xInputV = M::loadFromMemory( xInputPartA );
						typename M::Vector4F		yInputV = M::loadFromMemory( yInputPartA );
						typename M::Vector4F		kV = M::vectorizeOne( this->GetK() );
						typename M::Vector4F		aV = M::vectorizeOne( this->GetA() );
						typename M::Vector4F		F0V = M::loadFromMemory( F0InputPartA );
						typename M::Vector4F		omega0V = M::loadFromMemory( omega0InputPartA );
						typename M::Vector4F		wiV = M::loadFromMemory( wiInputPartA );

						xInputV = M::multiply( xInputV, kernelRadiusV );
						yInputV = M::multiply( yInputV, kernelRadiusV );

						typename M::Vector4F		gaborV = gaborVectorized( kV, aV, F0V, omega0V, xInputV, yInputV );
						gaborV = M::multiply( wiV, gaborV );

						VECTOR4_ALIGN( ValueType	gaborResult[ 4 ] );
						M::storeToMemory( gaborResult, gaborV );

						for( int j = 0; j < toCalculate; ++j )
						{
							noise += gaborResult[ j ];
						}

						toCalculate = 0;
					}
				}

				return noise;
			}

			inline
			unsigned int
			morton( unsigned int x, unsigned int y ) const
			{
				unsigned int	z = 0;

				for( unsigned int i = 0; i < (sizeof( unsigned int ) * CHAR_BIT); ++i )
				{
					z |= ((x & (1 << i)) << i) | ((y & (1 << i)) << (i + 1));
				}

				z += this->GetSeed();

				if( z == 0 )
				{
					z = 1;
				}

				return z;
			}

			inline
			typename M::Vector4I
			mortonVectorized( const typename M::Vector4I& xV, const typename M::Vector4I& yV ) const
			{
				typename M::Vector4I		zV = _mm_setzero_si128();
				typename M::Vector4I		oneV = M::vectorizeOne( 1 );
				typename M::Vector4I		seedV = M::vectorizeOne( this->GetSeed() );

				for( unsigned int i = 0; i < (sizeof( unsigned int ) * CHAR_BIT); ++i )
				{
					typename M::Vector4I		left = _mm_slli_epi32( oneV, i );
					left = _mm_and_si128( xV, left );
					left = _mm_slli_epi32( left, i );

					typename M::Vector4I		right = _mm_slli_epi32( oneV, i );
					right = _mm_and_si128( right, yV );
					right = _mm_slli_epi32( right, i + 1 );

					typename M::Vector4I		tmp = _mm_or_si128( left, right );
					zV = _mm_or_si128( zV, tmp );
				}

				zV = M::add( zV, seedV );

				typename M::Vector4I		isZero = _mm_cmpeq_epi32( zV, _mm_setzero_si128() );
				typename M::Vector4I		toOne = _mm_srli_epi32( isZero, 31 );
				zV = M::add( zV, toOne );

				return zV;
			}

			inline
			typename M::Vector4F
			gaborVectorized( const typename M::Vector4F& kV, const typename M::Vector4F& aV,
							 const typename M::Vector4F& f0V, const typename M::Vector4F& omega0V,
							 const typename M::Vector4F& xV, const typename M::Vector4F& yV ) const
			{
				typename M::Vector4F		piV = M::vectorizeOne( -M::Pi() );
				typename M::Vector4F		aa = M::multiply( aV, aV );
				typename M::Vector4F		xx = M::multiply( xV, xV );
				typename M::Vector4F		yy = M::multiply( yV, yV );
				typename M::Vector4F		xxyy = M::add( xx, yy );

				typename M::Vector4F		gaussianEnvelop =  M::multiply( piV, aa );
				gaussianEnvelop = M::multiply( gaussianEnvelop, xxyy );
				gaussianEnvelop = M::exp( gaussianEnvelop );
				gaussianEnvelop = M::multiply( gaussianEnvelop, kV );

				typename M::Vector4F		sinOmega;
				typename M::Vector4F		cosOmega;
				M::sinCos( omega0V, sinOmega, cosOmega );
				typename M::Vector4F		xCosOmega = M::multiply( xV, cosOmega );
				typename M::Vector4F		ySinOmega = M::multiply( yV, sinOmega );
				typename M::Vector4F		xCosYSin = M::add( xCosOmega, ySinOmega );

				typename M::Vector4F		sinusoidalCarrier = M::multiply( M::vectorizeOne( 2.0f ), piV );
				sinusoidalCarrier = M::multiply( sinusoidalCarrier, f0V );
				sinusoidalCarrier = M::multiply( sinusoidalCarrier, xCosYSin );
				sinusoidalCarrier = M::cos( sinusoidalCarrier );

				return M::multiply( gaussianEnvelop, sinusoidalCarrier );
			}

			inline
			ValueType
			variance() const
			{
				ValueType	K = this->GetK();
				ValueType	a = this->GetA();
				ValueType	F0 = this->GetF0();
				ValueType	impulseDensity = this->GetImpulseDensity();

				ValueType	integralGaborFilterSquared = ((K * K) / (ValueType( 4.0 ) * a * a)) * (ValueType( 1.0 ) + std::exp( -(ValueType( 2.0 ) * M::Pi() * F0 * F0) / (a * a) ));
				return impulseDensity * (ValueType( 1.0 ) / ValueType( 3.0 )) * integralGaborFilterSquared;
			}

			inline
			unsigned int
			findMax( unsigned int* a ) const
			{
				unsigned int	tmp1 = a[ 0 ] > a[ 1 ] ? a[ 0 ] : a[ 1 ];
				unsigned int	tmp2 = a[ 2 ] > a[ 3 ] ? a[ 2 ] : a[ 3 ];
				return tmp1 > tmp2 ? tmp1 : tmp2;
			}

		};

	}

}
