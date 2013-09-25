#pragma once


#include <climits>
#include <cmath>
#include <iostream>


#include <noise2/math/Math.hpp>
#include <noise2/module/Module.hpp>
#include <noise2/module/gabor/GaborBase.hpp>
#include <noise2/module/gabor/Prng.hpp>
#include <noise2/module/gabor/PrngVector.hpp>

#include <noise2/debug/Debug.hpp>




namespace noise2
{

	namespace module
	{

		namespace gabor
		{

			template< typename ValueT >
			class GaborImpl< ValueT, 2, 4 >: public Module< ValueT, 2 >, public gabor::GaborBase< ValueT >
			{

			public:

				typedef ValueT									ValueType;
				static const unsigned							Dimension = 2;
				typedef Module< ValueType, Dimension >			ModuleType;
				typedef gabor::GaborBase< ValueType >			BaseType;
				typedef GaborImpl< ValueType, Dimension, 4 >	ThisType;



			private:
				typedef gabor::Prng< ValueType >			PrngType;
				typedef gabor::PrngVector< ValueType >		PrngVectorType;
				typedef math::Math< ValueType >				M;



			public:

				GaborImpl():
				  ModuleType( 0 ),
				  BaseType()
				{}

				virtual
				~GaborImpl()
				{}

				virtual
				ValueType
				GetValue( ValueType x, ValueType y ) const
				{
					ValueType	noise = ValueType( 0.0 );

					for( size_t w = 0; w < this->preparedWidgets.size(); ++w )
					{
						const typename BaseType::PreparedWidget&	widget = this->preparedWidgets[ w ];

						ValueType	widgetX = x / widget.kernelRadius;
						ValueType	widgetY = y / widget.kernelRadius;

						ValueType	intX = M::floor( widgetX );
						ValueType	intY = M::floor( widgetY );
						ValueType	fracX = widgetX - intX;
						ValueType	fracY = widgetY - intY;
						int32		i = int32( intX );
						int32		j = int32( intY );

						typename M::Vector4F	fracXV = M::vectorizeOne( widgetX - intX );
						typename M::Vector4F	fracYV = M::vectorizeOne( widgetY - intY );

						int*	diA;
						int*	djA;

						static VECTOR4_ALIGN( int		diA_LL[ 9 ] ) = { 0, -1, 0, -1, 1, 1, 0, -1, 1 };
						static VECTOR4_ALIGN( int		djA_LL[ 9 ] ) = { 0, -1, -1, 0, 0, -1, 1, 1, 1 };
						static VECTOR4_ALIGN( int		diA_UR[ 9 ] ) = { 0, 1, 0, 1, 0, 1, -1, -1, -1 };
						static VECTOR4_ALIGN( int		djA_UR[ 9 ] ) = { 0, 1, 1, 0, -1, -1, 0, 1, -1 };
						static VECTOR4_ALIGN( int		diA_UL[ 9 ] ) = { 0, -1, -1, 0, 1, 1, 0, -1, 1 };
						static VECTOR4_ALIGN( int		djA_UL[ 9 ] ) = { 0, 1, 0, 1, 0, 1, -1, -1, -1 };
						static VECTOR4_ALIGN( int		diA_LR[ 9 ] ) = { 0, 1, 0, 1, 0, 1, -1, -1, -1 };
						static VECTOR4_ALIGN( int		djA_LR[ 9 ] ) = { 0, -1, -1, 0, 1, 1, 0, -1, 1 };

						if( fracX < ValueType( 0.5 ) )
						{
							if( fracY < ValueType( 0.5 ) )
							{
								diA = diA_LL;
								djA = djA_LL;
							}
							else
							{
								diA = diA_UL;
								djA = djA_UL;
							}
						}
						else
						{
							if( fracY < ValueType( 0.5 ) )
							{
								diA = diA_LR;
								djA = djA_LR;
							}
							else
							{
								diA = diA_UR;
								djA = djA_UR;
							}
						}

						typename M::Vector4I		iV = M::vectorizeOne( i );
						typename M::Vector4I		jV = M::vectorizeOne( j );

						for( int32 m = 0; m < 2; ++m )
						{
							typename M::Vector4I	diV = M::loadFromMemory( diA + (m * 4) );
							typename M::Vector4I	djV = M::loadFromMemory( djA + (m * 4) );
							typename M::Vector4I	diiV = M::add( diV, iV );
							typename M::Vector4I	djjV = M::add( djV, jV );
							typename M::Vector4F	xV = M::subtract( fracXV, M::intToFloat( diV ) );
							typename M::Vector4F	yV = M::subtract( fracYV, M::intToFloat( djV ) );
							typename M::Vector4I	mortonV = mortonVectorized( widget, diiV, djjV );
							
							noise += cellVectorized( widget, mortonV, xV, yV );
						}

						for( int32 c = 8; c < 9; ++c )
						{
							uint32	s = morton( widget, diA[ c ] + i, djA[ c ] + j );
							noise += cell( widget, s, fracX - diA[ c ], fracY - djA[ c ] );
						}
					}

					return noise / (ValueType( 3.0 ) * std::sqrt( this->combinedVariance ));
				}

				virtual
				void
				GetValue4( const ValueType* inputX, const ValueType* inputY, ValueType* output ) const
				{
					typename M::Vector4F	noiseV = M::constZeroF();
					
					for( size_t w = 0; w < this->preparedWidgets.size(); ++w )
					{
						const typename BaseType::PreparedWidget&	widget = this->preparedWidgets[ w ];

						typename M::Vector4F	xV = M::loadFromMemory( inputX );
						typename M::Vector4F	yV = M::loadFromMemory( inputY );

						xV = M::divide( xV, M::vectorizeOne( widget.kernelRadius ) );
						yV = M::divide( yV, M::vectorizeOne( widget.kernelRadius ) );
						
						typename M::Vector4F	intXV = M::floor( xV );
						typename M::Vector4F	intYV = M::floor( yV );
						typename M::Vector4F	fracXV = M::subtract( xV, intXV );
						typename M::Vector4F	fracYV = M::subtract( yV, intYV );
						typename M::Vector4I	iV = M::floatToIntTruncated( intXV );
						typename M::Vector4I	jV = M::floatToIntTruncated( intYV );

						static VECTOR4_ALIGN( int32		diA[ 36 ] ) = { -1, -1, -1, -1,
																		-1, -1, -1, -1,
																		-1, -1, -1, -1,
																		 0,  0,  0,  0,
																		 0,  0,  0,  0,
																		 0,  0,  0,  0,
																		 1,  1,  1,  1,
																		 1,  1,  1,  1,
																		 1,  1,  1,  1 };
						static VECTOR4_ALIGN( int32		djA[ 36 ] ) = { -1, -1, -1, -1,
																		 0,  0,  0,  0,
																		 1,  1,  1,  1,
																		-1, -1, -1, -1,
																		 0,  0,  0,  0,
																		 1,  1,  1,  1,
																		 0,  0,  0,  0,
																		 1,  1,  1,  1,
																		-1, -1, -1, -1 };					

						for( int32 m = 0; m < 9; ++m )
						{
							typename M::Vector4I	diV = M::loadFromMemory( diA + (m * 4) );
							typename M::Vector4I	djV = M::loadFromMemory( djA + (m * 4) );
							typename M::Vector4I	diiV = M::add( diV, iV );
							typename M::Vector4I	djjV = M::add( djV, jV );
							typename M::Vector4F	fxV = M::subtract( fracXV, M::intToFloat( diV ) );
							typename M::Vector4F	fyV = M::subtract( fracYV, M::intToFloat( djV ) );
							typename M::Vector4I	mortonV = mortonVectorized( widget, diiV, djjV );
							
							noiseV = M::add( noiseV, cell4Vectorized( widget, mortonV, fxV, fyV ) );
						}
					}

					noiseV = M::divide( noiseV, M::vectorizeOne( ValueType( 3.0 ) * std::sqrt( this->combinedVariance ) ) );
					M::storeToMemory( output, noiseV );
				}



			private:

				inline
				ValueType
				cell( const typename BaseType::PreparedWidget& widget, uint32 s, ValueType x, ValueType y ) const
				{
					ValueType		noise = ValueType( 0.0 );

					PrngType	prng( s );
					ValueType	numberOfImpulsesPerCell = widget.impulseDensity * widget.kernelRadius * widget.kernelRadius;
					uint32		numberOfImpulses = prng.poisson( numberOfImpulsesPerCell );

					uint32		toCalculate = 0;
					VECTOR4_ALIGN( ValueType	xInputA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
					VECTOR4_ALIGN( ValueType	yInputA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
					VECTOR4_ALIGN( ValueType	wiInputA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
					VECTOR4_ALIGN( ValueType	F0InputA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
					VECTOR4_ALIGN( ValueType	omega0InputA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };


					for( uint32 i = 0; i < numberOfImpulses; ++i )
					{
						ValueType	xi = prng.uniformNormalized();
						ValueType	yi = prng.uniformNormalized();
						ValueType	wi = prng.uniformRangeMinusOneToOne();
						ValueType	F0 = prng.uniformRange( widget.frequencyRangeStart, widget.frequencyRangeEnd );
						ValueType	omega0 = prng.uniformRange( widget.angularRangeStart, widget.angularRangeEnd );
						ValueType	xix = x - xi;
						ValueType	yiy = y - yi;

						if( ((xix * xix) + (yiy * yiy)) < ValueType( 1.0 ) )
						{
							xInputA[ toCalculate ] = xix * widget.kernelRadius;
							yInputA[ toCalculate ] = yiy * widget.kernelRadius;
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
							typename M::Vector4F		kV = M::vectorizeOne( widget.K );
							typename M::Vector4F		aV = M::vectorizeOne( widget.a );
							typename M::Vector4F		F0V = M::loadFromMemory( F0InputA );
							typename M::Vector4F		omega0V = M::loadFromMemory( omega0InputA );
							typename M::Vector4F		wiV = M::loadFromMemory( wiInputA );

							typename M::Vector4F		gaborV = gaborVectorized( kV, aV, F0V, omega0V, xInputV, yInputV );
							gaborV = M::multiply( wiV, gaborV );

							VECTOR4_ALIGN( ValueType	gaborResult[ 4 ] );
							M::storeToMemory( gaborResult, gaborV );

							for( uint32 j = 0; j < toCalculate; ++j )
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
				cellVectorized( const typename BaseType::PreparedWidget& widget, const typename M::Vector4I sV, const typename M::Vector4F xV, const typename M::Vector4F yV ) const
				{
					Compacter	compacter;

					PrngVectorType			prngVector( sV );
					typename M::Vector4F	impulseDensityV = M::vectorizeOne( widget.impulseDensity );
					typename M::Vector4F	kernelRadiusV = M::vectorizeOne( widget.kernelRadius );
					typename M::Vector4F	numberOfImpulsesPerCellV = M::multiply( impulseDensityV, M::multiply( kernelRadiusV, kernelRadiusV ) );
					typename M::Vector4I	numberOfImpulsesV = prngVector.poisson( numberOfImpulsesPerCellV );

					VECTOR4_ALIGN( uint32	numberOfImpulsesA[ 4 ] );
					M::storeToMemory( numberOfImpulsesA, numberOfImpulsesV );

					uint32					maxNumberOfImpulses = findMax( numberOfImpulsesA );
					typename M::Vector4I	oneIV = M::constOneI();
					typename M::Vector4F	oneFV = M::constOneF();

					typename M::Vector4F	noiseV = M::constZeroF();
					for( uint32 i = 0; i < maxNumberOfImpulses; ++i )
					{
						typename M::Vector4I	impulseMaskV = M::greaterThan( numberOfImpulsesV, M::constZeroI() );
						typename M::Vector4F	xiV = prngVector.uniformNormalized();
						typename M::Vector4F	yiV = prngVector.uniformNormalized();
						typename M::Vector4F	wiV = prngVector.uniformRangeMinusOneToOne();
						typename M::Vector4F	F0V = prngVector.uniformRange( widget.frequencyRangeStart, widget.frequencyRangeEnd );
						typename M::Vector4F	omega0V = prngVector.uniformRange( widget.angularRangeStart, widget.angularRangeEnd );
						typename M::Vector4F	xInputV = M::subtract( xV, xiV );
						typename M::Vector4F	yInputV = M::subtract( yV, yiV );

						typename M::Vector4F	radiusXV = M::multiply( xInputV, xInputV );
						typename M::Vector4F	radiusYV = M::multiply( yInputV, yInputV );
						typename M::Vector4F	radiusV = M::add( radiusXV, radiusYV );
						typename M::Vector4I	radiusMaskV = M::castToInt( M::lowerThan( radiusV, oneFV ) );
						typename M::Vector4I	calcMaskV = M::bitAnd( radiusMaskV, impulseMaskV );

						if( compacter.compact( xInputV, yInputV, wiV, F0V, omega0V, calcMaskV ) == true )
						{
							noiseV = M::add( noiseV, cellPart( widget, xInputV, yInputV, wiV, F0V, omega0V, calcMaskV ) );
						}
						if( i == maxNumberOfImpulses - 1 )
						{
							noiseV = M::add( noiseV, cellPart( widget,
															   compacter.xInputVPrev,
															   compacter.yInputVPrev,
															   compacter.wiVPrev,
															   compacter.F0VPrev,
															   compacter.omega0VPrev,
															   compacter.calcMaskVPrev ) );
						}

						numberOfImpulsesV = M::subtract( numberOfImpulsesV, M::bitAnd( impulseMaskV, oneIV ) );
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
				cell4Vectorized( const typename BaseType::PreparedWidget& widget, const typename M::Vector4I sV, const typename M::Vector4F xV, const typename M::Vector4F yV ) const
				{
					PrngVectorType			prngVector( sV );
					typename M::Vector4F	impulseDensityV = M::vectorizeOne( widget.impulseDensity );
					typename M::Vector4F	kernelRadiusV = M::vectorizeOne( widget.kernelRadius );
					typename M::Vector4F	numberOfImpulsesPerCellV = M::multiply( impulseDensityV, M::multiply( kernelRadiusV, kernelRadiusV ) );
					typename M::Vector4I	numberOfImpulsesV = prngVector.poisson( numberOfImpulsesPerCellV );

					VECTOR4_ALIGN( uint32	numberOfImpulsesA[ 4 ] );
					M::storeToMemory( numberOfImpulsesA, numberOfImpulsesV );

					uint32					maxNumberOfImpulses = findMax( numberOfImpulsesA );
					typename M::Vector4I	oneIV = M::constOneI();
					typename M::Vector4F	oneFV = M::constOneF();

					typename M::Vector4F	noiseV = M::constZeroF();
					for( uint32 i = 0; i < maxNumberOfImpulses; ++i )
					{
						typename M::Vector4I	impulseMaskV = M::greaterThan( numberOfImpulsesV, M::constZeroI() );
						typename M::Vector4F	xiV = prngVector.uniformNormalized();
						typename M::Vector4F	yiV = prngVector.uniformNormalized();
						typename M::Vector4F	wiV = prngVector.uniformRangeMinusOneToOne();
						typename M::Vector4F	F0V = prngVector.uniformRange( widget.frequencyRangeStart, widget.frequencyRangeEnd );
						typename M::Vector4F	omega0V = prngVector.uniformRange( widget.angularRangeStart, widget.angularRangeEnd );
						typename M::Vector4F	xInputV = M::subtract( xV, xiV );
						typename M::Vector4F	yInputV = M::subtract( yV, yiV );

						typename M::Vector4F	radiusXV = M::multiply( xInputV, xInputV );
						typename M::Vector4F	radiusYV = M::multiply( yInputV, yInputV );
						typename M::Vector4F	radiusV = M::add( radiusXV, radiusYV );
						typename M::Vector4I	radiusMaskV = M::castToInt( M::lowerThan( radiusV, oneFV ) );
						typename M::Vector4I	calcMaskV = M::bitAnd( radiusMaskV, impulseMaskV );

						if( M::isAllZeros( calcMaskV ) == false )
						{
							noiseV = M::add( noiseV, cellPart( widget, xInputV, yInputV, wiV, F0V, omega0V, calcMaskV ) );
						}

						numberOfImpulsesV = M::subtract( numberOfImpulsesV, M::bitAnd( impulseMaskV, oneIV ) );
					}

					return noiseV;
				}

				inline
				typename M::Vector4F
				cellPart( const typename BaseType::PreparedWidget& widget,
						  const typename M::Vector4F& xInputV, const typename M::Vector4F& yInputV,
						  const typename M::Vector4F& wiV, const typename M::Vector4F& F0V,
						  const typename M::Vector4F& omega0V, const typename M::Vector4I& calcMaskV ) const
				{
					typename M::Vector4F	kernelRadiusV = M::vectorizeOne( widget.kernelRadius );
					typename M::Vector4F	xV = M::multiply( xInputV, kernelRadiusV );
					typename M::Vector4F	yV = M::multiply( yInputV, kernelRadiusV );
					typename M::Vector4F	kV = M::vectorizeOne( widget.K );
					typename M::Vector4F	aV = M::vectorizeOne( widget.a );

					typename M::Vector4F	gaborV = gaborVectorized( kV, aV, F0V, omega0V, xV, yV );
					gaborV = M::multiply( wiV, gaborV );
					gaborV = M::select( gaborV, calcMaskV );

					return gaborV;
				}

				inline
				uint32
				morton( const typename BaseType::PreparedWidget& widget, uint32 x, uint32 y ) const
				{
					uint32	z = 0;

					for( uint32 i = 0; i < (sizeof( uint32 ) * CHAR_BIT); ++i )
					{
						z |= ((x & (1 << i)) << i) | ((y & (1 << i)) << (i + 1));
					}

					z += widget.seed;

					if( z == 0 )
					{
						z = 1;
					}

					return z;
				}

				inline
				typename M::Vector4I
				mortonVectorized( const typename BaseType::PreparedWidget& widget, const typename M::Vector4I& xV, const typename M::Vector4I& yV ) const
				{
					typename M::Vector4I		zV = M::constZeroI();
					typename M::Vector4I		oneV = M::constOneI();
					typename M::Vector4I		seedV = M::vectorizeOne( widget.seed );

					for( uint32 i = 0; i < (sizeof( uint32 ) * CHAR_BIT); ++i )
					{
						typename M::Vector4I		left = M::shiftLeftLogical( oneV, i );
						left = M::bitAnd( xV, left );
						left = M::shiftLeftLogical( left, i );

						typename M::Vector4I		right = M::shiftLeftLogical( oneV, i );
						right = M::bitAnd( right, yV );
						right = M::shiftLeftLogical( right, i + 1 );

						typename M::Vector4I		tmp = M::bitOr( left, right );
						zV = M::bitOr( zV, tmp );
					}

					zV = M::add( zV, seedV );

					typename M::Vector4I		isZero = M::equal( zV, M::constZeroI() );
					typename M::Vector4I		toOne = M::shiftRightLogical( isZero, 31 );
					zV = M::add( zV, toOne );

					return zV;
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
