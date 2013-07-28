#pragma once


#include <climits>
#include <cmath>
#include <iostream>


#include <noise2/math/Math.hpp>
#include <noise2/module/gabor/GaborBase.hpp>
#include <noise2/module/gabor/PrngVector.hpp>
#include <noise2/proto/gaborSurface/Projection_Vector.hpp>

#include <noise2/debug/Debug.hpp>




namespace noise2
{

	namespace proto
	{

		namespace gaborSurface
		{

			template< typename ValueT >
			class GaborSurfaceImpl< ValueT, 4 >: public module::gabor::GaborBase< ValueT >
			{

			public:

				typedef ValueT									ValueType;
				typedef module::gabor::GaborBase< ValueType >	BaseType;
				typedef GaborSurfaceImpl< ValueType, 4 >		ThisType;



			private:
				typedef module::gabor::PrngVector< ValueType >		PrngVectorType;
				typedef math::Math< ValueType >						M;



			public:

				GaborSurfaceImpl():
				  BaseType()
				{}

				virtual
				~GaborSurfaceImpl()
				{}

				ValueType
				GetValue( ValueType pX, ValueType pY, ValueType pZ,
						  ValueType nX, ValueType nY, ValueType nZ ) const
				{
					pX /= this->GetKernelRadius();
					pY /= this->GetKernelRadius();
					pZ /= this->GetKernelRadius();

					ValueType	intX = M::floor( pX );
					ValueType	intY = M::floor( pY );
					ValueType	intZ = M::floor( pZ );
					int32		i = int( intX );
					int32		j = int( intY );
					int32		k = int( intZ );
					ValueType	noise = ValueType( 0.0 );

					typename M::Vector4F	nxV = M::vectorizeOne( nX );
					typename M::Vector4F	nyV = M::vectorizeOne( nY );
					typename M::Vector4F	nzV = M::vectorizeOne( nZ );
					typename M::Vector4F	fracXV = M::vectorizeOne( pX - intX );
					typename M::Vector4F	fracYV = M::vectorizeOne( pY - intY );
					typename M::Vector4F	fracZV = M::vectorizeOne( pZ - intZ );

					static VECTOR4_ALIGN( int32		diA[ 28 ] ) = { -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0 };
					static VECTOR4_ALIGN( int32		djA[ 28 ] ) = { -1, -1, -1,  0,  0,  0,  1,  1,  1, -1, -1, -1,  0,  0,  0,  1,  1,  1, -1, -1, -1,  0,  0,  0,  1,  1,  1,  0 };
					static VECTOR4_ALIGN( int32		dkA[ 28 ] ) = { -1,  0,  1, -1,  0,  1, -1,  0,  1, -1,  0,  1, -1,  0,  1, -1,  0,  1, -1,  0,  1, -1,  0,  1, -1,  0,  1,  0 };

					typename M::Vector4I		iV = M::vectorizeOne( i );
					typename M::Vector4I		jV = M::vectorizeOne( j );
					typename M::Vector4I		kV = M::vectorizeOne( k );

					typename M::Vector4I	diV;
					typename M::Vector4I	djV;
					typename M::Vector4I	dkV;
					typename M::Vector4I	diiV;
					typename M::Vector4I	djjV;
					typename M::Vector4I	dkkV;
					typename M::Vector4F	xV;
					typename M::Vector4F	yV;
					typename M::Vector4F	zV;
					typename M::Vector4I	mortonV;
					for( int32 m = 0; m < 6; ++m )
					{
						diV = M::loadFromMemory( diA + (m * 4) );
						djV = M::loadFromMemory( djA + (m * 4) );
						dkV = M::loadFromMemory( dkA + (m * 4) );
						diiV = M::add( diV, iV );
						djjV = M::add( djV, jV );
						dkkV = M::add( dkV, kV );
						xV = M::subtract( fracXV, M::intToFloat( diV ) );
						yV = M::subtract( fracYV, M::intToFloat( djV ) );
						zV = M::subtract( fracZV, M::intToFloat( dkV ) );
						mortonV = mortonVectorized( diiV, djjV, dkkV );
						
						noise += cellVectorized( mortonV, xV, yV, zV, nxV, nyV, nzV, M::constFullMaskI() );
					}
					{
						diV = M::loadFromMemory( diA + (6 * 4) );
						djV = M::loadFromMemory( djA + (6 * 4) );
						dkV = M::loadFromMemory( dkA + (6 * 4) );
						diiV = M::add( diV, iV );
						djjV = M::add( djV, jV );
						dkkV = M::add( dkV, kV );
						xV = M::subtract( fracXV, M::intToFloat( diV ) );
						yV = M::subtract( fracYV, M::intToFloat( djV ) );
						zV = M::subtract( fracZV, M::intToFloat( dkV ) );
						mortonV = mortonVectorized( diiV, djjV, dkkV );
						
						static VECTOR4_ALIGN( uint32	cell3MaskA[ 4 ] ) = { 0xffffffff, 0xffffffff, 0xffffffff, 0x0 };
						noise += cellVectorized( mortonV, xV, yV, zV, nxV, nyV, nzV, M::loadFromMemory( cell3MaskA ) );
					}

					return noise / (ValueType( 3.0 ) * std::sqrt( variance() ));
				}



			private:

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
					  xInputVPrev( M::constZeroF() ),
					  yInputVPrev( M::constZeroF() ),
					  wiVPrev( M::constZeroF() ),
					  F0VPrev( M::constZeroF() ),
					  omega0VPrev( M::constZeroF() ),
					  calcMaskVPrev( M::constZeroI() )
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

						xInputVPrev = M::shiftRightLoop( xInputVPrev, 1 );
						yInputVPrev = M::shiftRightLoop( yInputVPrev, 1 );
						wiVPrev = M::shiftRightLoop( wiVPrev, 1 );
						F0VPrev = M::shiftRightLoop( F0VPrev, 1 );
						omega0VPrev = M::shiftRightLoop( omega0VPrev, 1 );
						calcMaskVPrev = M::shiftRightLoop( calcMaskVPrev, 1 );

						typename M::Vector4I	compactMaskV = M::bitXor( calcMaskVPrev, M::bitOr( calcMaskVPrev, calcMaskVNext ) );
						typename M::Vector4I	keepMaskV = M::bitAnd( calcMaskVPrev, calcMaskVNext );
						bool	calculate = !M::isAllZeros( keepMaskV );
						
						xInputVNext = compactOne( compactMaskV, keepMaskV, xInputVPrev, xInputVNext, calculate );
						yInputVNext = compactOne( compactMaskV, keepMaskV, yInputVPrev, yInputVNext, calculate );
						wiVNext = compactOne( compactMaskV, keepMaskV, wiVPrev, wiVNext, calculate );
						F0VNext = compactOne( compactMaskV, keepMaskV, F0VPrev, F0VNext, calculate );
						omega0VNext = compactOne( compactMaskV, keepMaskV, omega0VPrev, omega0VNext, calculate );

						if( calculate == false )
						{
							calcMaskVPrev = M::bitOr( calcMaskVPrev, calcMaskVNext );
						}
						else
						{
							calcMaskVNext = M::bitOr( calcMaskVPrev, calcMaskVNext );
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
						typename M::Vector4F	compactV = M::blend( vPrev, vNext, compactMaskV );
						typename M::Vector4F	keepV = M::select( vNext, keepMaskV );
						
						if( calculate == false )
						{
							vPrev = compactV;
							return keepV;
						}
						else
						{
							vPrev = keepV;
							return compactV;
						}
					}

				};

				inline
				ValueType
				cellVectorized( const typename M::Vector4I sV,
								const typename M::Vector4F pxV, const typename M::Vector4F pyV, const typename M::Vector4F pzV,
								const typename M::Vector4F nxV, const typename M::Vector4F nyV, const typename M::Vector4F nzV,
								const typename M::Vector4I cellMaskV ) const
				{
					Compacter	compacter;

					PrngVectorType			prngVector( sV );
					typename M::Vector4F	impulseDensityV = M::vectorizeOne( this->GetImpulseDensity() );
					typename M::Vector4F	kernelRadiusV = M::vectorizeOne( this->GetKernelRadius() );
					typename M::Vector4F	numberOfImpulsesPerCellV = M::multiply( impulseDensityV, M::multiply( kernelRadiusV, kernelRadiusV ) );
					numberOfImpulsesPerCellV = M::divide( numberOfImpulsesPerCellV, M::constTwoF() );
					typename M::Vector4I	numberOfImpulsesV = prngVector.poisson( numberOfImpulsesPerCellV );
					numberOfImpulsesV = M::bitAnd( numberOfImpulsesV, cellMaskV );

					VECTOR4_ALIGN( uint32	numberOfImpulsesA[ 4 ] );
					M::storeToMemory( numberOfImpulsesA, numberOfImpulsesV );
					uint32	maxNumberOfImpulses = findMax( numberOfImpulsesA );
					
					ProjectionVector< ValueType >	projection( nxV, nyV, nzV,
																pxV, pyV, pzV );

					typename M::Vector4F	noiseV = M::constZeroF();
					for( uint32 i = 0; i < maxNumberOfImpulses; ++i )
					{
						typename M::Vector4I	impulseMaskV = M::greaterThan( numberOfImpulsesV, M::constZeroI() );
						typename M::Vector4F	xiV = prngVector.uniformNormalized();
						typename M::Vector4F	yiV = prngVector.uniformNormalized();
						typename M::Vector4F	ziV = prngVector.uniformNormalized();
						typename M::Vector4F	F0V = prngVector.uniformRange( this->GetFrequencyRangeStart(), this->GetFrequencyRangeEnd() );
						typename M::Vector4F	omega0V = prngVector.uniformRange( this->GetAngularRangeStart(), this->GetAngularRangeEnd() );

						projection.project( xiV, yiV, ziV );
						ziV = M::subtract( M::constOneF(), M::abs( ziV ) );
						
						typename M::Vector4F	radiusXV = M::multiply( xiV, xiV );
						typename M::Vector4F	radiusYV = M::multiply( yiV, yiV );
						typename M::Vector4F	radiusV = M::add( radiusXV, radiusYV );
						typename M::Vector4I	radiusMaskV = M::castToInt( M::lowerThan( radiusV, M::constOneF() ) );

						typename M::Vector4I	distanceMaskV = M::castToInt( M::equalGreaterThan( ziV, M::constZeroF() ) );

						typename M::Vector4I	calcMaskV = M::bitAnd( radiusMaskV, distanceMaskV );
						calcMaskV = M::bitAnd( calcMaskV, impulseMaskV );
						
						if( compacter.compact( xiV, yiV, ziV, F0V, omega0V, calcMaskV ) == true )
						{
							noiseV = M::add( noiseV, cellPart( xiV, yiV, ziV, F0V, omega0V, calcMaskV ) );
						}
						if( i == maxNumberOfImpulses - 1 )
						{
							noiseV = M::add( noiseV, cellPart( compacter.xInputVPrev,
															   compacter.yInputVPrev,
															   compacter.wiVPrev,
															   compacter.F0VPrev,
															   compacter.omega0VPrev,
															   compacter.calcMaskVPrev ) );
						}

						numberOfImpulsesV = M::subtract( numberOfImpulsesV, M::bitAnd( impulseMaskV, M::constOneI() ) );
					}

					VECTOR4_ALIGN( typename M::ScalarF		noiseA[ 4 ] );
					M::storeToMemory( noiseA, noiseV );
					ValueType	noise = ValueType( 0.0 );
					
					for( int j = 0; j < 4; ++j )
					{
						noise += noiseA[ j ];
					}
					
					return noise;
				}

				inline
				typename M::Vector4F
				cellPart( const typename M::Vector4F& xInputV, const typename M::Vector4F& yInputV,
						  const typename M::Vector4F& wiV, const typename M::Vector4F& F0V,
						  const typename M::Vector4F& omega0V, const typename M::Vector4I& calcMaskV ) const
				{
					typename M::Vector4F	kernelRadiusV = M::vectorizeOne( this->GetKernelRadius() );
					typename M::Vector4F	xV = M::multiply( xInputV, kernelRadiusV );
					typename M::Vector4F	yV = M::multiply( yInputV, kernelRadiusV );
					typename M::Vector4F	kV = M::vectorizeOne( this->GetK() );
					typename M::Vector4F	aV = M::vectorizeOne( this->GetA() );

					typename M::Vector4F	gaborV = gaborVectorized( kV, aV, F0V, omega0V, xV, yV );
					gaborV = M::multiply( wiV, gaborV );
					gaborV = M::select( gaborV, calcMaskV );

					return gaborV;
				}

				inline
				typename M::Vector4I
				mortonVectorized( const typename M::Vector4I& xV, const typename M::Vector4I& yV, const typename M::Vector4I& zV ) const
				{
					typename M::Vector4I		mV = M::constZeroI();
					typename M::Vector4I		oneV = M::constOneI();
					typename M::Vector4I		seedV = M::vectorizeOne( this->GetSeed() );

					for( uint32 i = 0; i < (2 * CHAR_BIT); ++i )
					{
						typename M::Vector4I		xPart = M::shiftLeftLogical( oneV, i );
						xPart = M::bitAnd( xPart, xV );
						xPart = M::shiftLeftLogical( xPart, i * 2 );

						typename M::Vector4I		yPart = M::shiftLeftLogical( oneV, i );
						yPart = M::bitAnd( yPart, yV );
						yPart = M::shiftLeftLogical( yPart, (i * 2) + 1 );

						typename M::Vector4I		tmp = M::bitOr( xPart, yPart );

						typename M::Vector4I		zPart = M::shiftLeftLogical( oneV, i );
						zPart = M::bitAnd( zPart, zV );
						zPart = M::shiftLeftLogical( zPart, (i * 2) + 2 );

						tmp = M::bitOr( tmp, zPart );

						mV = M::bitOr( mV, tmp );
					}

					mV = M::add( mV, seedV );

					typename M::Vector4I		isZero = M::equal( mV, M::constZeroI() );
					typename M::Vector4I		toOne = M::shiftRightLogical( isZero, 31 );
					mV = M::add( mV, toOne );

					return mV;
				}

				inline
				typename M::Vector4F
				gaborVectorized( const typename M::Vector4F& kV, const typename M::Vector4F& aV,
								 const typename M::Vector4F& f0V, const typename M::Vector4F& omega0V,
								 const typename M::Vector4F& xV, const typename M::Vector4F& yV ) const
				{
					typename M::Vector4F		piV = M::constMinusPiF();
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
					
					typename M::Vector4F		sinusoidalCarrier = M::multiply( M::constTwoF(), piV );
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

					ValueType	integralGaborFilterSquared = ((K * K) / (ValueType( 4.0 ) * a * a)) * (ValueType( 1.0 ) + M::exp( -(ValueType( 2.0 ) * M::Pi() * F0 * F0) / (a * a) ));
					return impulseDensity * (ValueType( 1.0 ) / ValueType( 3.0 )) * integralGaborFilterSquared;
				}

				inline
				uint32
				findMax( uint32* a ) const
				{
					uint32	tmp1 = a[ 0 ] > a[ 1 ] ? a[ 0 ] : a[ 1 ];
					uint32	tmp2 = a[ 2 ] > a[ 3 ] ? a[ 2 ] : a[ 3 ];
					return tmp1 > tmp2 ? tmp1 : tmp2;
				}

			};

		}

	}

}
