#pragma once


#include <climits>
#include <cmath>
#include <iostream>


#include <noise/Math.hpp>
#include <noise/module/Module.hpp>
#include <noise/module/gabor/GaborBase.hpp>
#include <noise/module/gabor/PRNG.hpp>
#include <noise/module/gabor/PRNG_Vector.hpp>











namespace noise
{

	namespace module
	{

		template< >
		class Gabor< float, 2 >: public Module< float, 2 >, public gabor::GaborBase< float >
		{

		public:

			typedef float								ValueType;
			static const unsigned						Dimension = 2;
			typedef Module< ValueType, Dimension >		ModuleType;
			typedef gabor::GaborBase< ValueType >		BaseType;
			typedef Gabor< ValueType, Dimension >		ThisType;
			typedef gabor::PRNG< ValueType >			PRNGType;
			typedef gabor::PRNG_Vector< ValueType >		PRNG_VectorType;
			typedef Math< ValueType >					M;



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

				static VECTOR_ALIGN( int		diA[ 12 ] ) = { -1, -1, -1, 0, 0, 0, 1, 1, 1, 0, 0, 0 };
				static VECTOR_ALIGN( int		djA[ 12 ] ) = { -1, 0, 1, -1, 0, 1, -1, 0, 1, 0, 0, 0 };

				VECTOR_ALIGN( unsigned int	mortonA[ 12 ] );
				
				M::Vector4I		seedV = M::vectorizeOne( (M::ScalarI) this->GetSeed() );
				M::Vector4I		iV = M::vectorizeOne( (M::ScalarI) i );
				M::Vector4I		jV = M::vectorizeOne( (M::ScalarI) j );

				ValueType	poissonG = std::exp( -(this->GetImpulseDensity() * this->GetKernelRadius() * this->GetKernelRadius()) );

				for( int m = 0; m < 3; ++m )
				{
					M::Vector4I		diV = M::loadFromMemory( diA + (m * 4) );
					M::Vector4I		djV = M::loadFromMemory( djA + (m * 4) );
					M::Vector4I		diiV = M::add( diV, iV );
					M::Vector4I		djjV = M::add( djV, jV );

					M::Vector4I		mortonV = mortonVectorized( diiV, djjV );
					M::storeToMemory( (M::ScalarI*) mortonA + (m * 4), mortonV );
				}

				VECTOR_ALIGN( unsigned int	sA[ 9 ] );
				VECTOR_ALIGN( unsigned int	numberOfImpulsesA[ 9 ] );
				VECTOR_ALIGN( ValueType		xA[ 9 ] );
				VECTOR_ALIGN( ValueType		yA[ 9 ] );
				for( int c = 0; c < 9; ++c )
				{
					unsigned int	s = mortonA[ c ];
					if( s == 0 )
					{
						s = 1;
					}

					PRNGType		prng( s );
					numberOfImpulsesA[ c ] = prng.poissonG( poissonG );
					sA[ c ] = prng.getSeed();
					xA[ c ] = fracX - diA[ c ];
					yA[ c ] = fracY - djA[ c ];
				}

				noise += cellVectorized( sA, numberOfImpulsesA, xA, yA );
				noise += cellVectorized( sA + 4, numberOfImpulsesA + 4, xA + 4, yA + 4 );
				for( int c = 8; c < 9; ++c )
				{
					noise += cell( sA[ c ], numberOfImpulsesA[ c ], xA[ c ], yA[ c ] );
				}

				return noise / (ValueType( 3.0 ) * std::sqrt( variance() ));
			}



		private:

			inline
			ValueType
			cell( unsigned int s, unsigned int numberOfImpulses, ValueType x, ValueType y ) const
			{
				PRNGType	prng( s );
				ValueType	noise = ValueType( 0.0 );

				int		toCalculate = 0;
				SSE_ALIGN( ValueType	xInput[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
				SSE_ALIGN( ValueType	yInput[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
				SSE_ALIGN( ValueType	wiVector[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };

				for( unsigned int i = 0; i < numberOfImpulses; ++i )
				{
					ValueType	xi = prng.uniformNormalized();
					ValueType	yi = prng.uniformNormalized();
					ValueType	wi = prng.uniformRange( -1.0, +1.0 );
					ValueType	xix = x - xi;
					ValueType	yiy = y - yi;

					if( ((xix * xix) + (yiy * yiy)) < ValueType( 1.0 ) )
					{
						xInput[ toCalculate ] = xix * this->GetKernelRadius();
						yInput[ toCalculate ] = yiy * this->GetKernelRadius();
						wiVector[ toCalculate ] = wi;
						++toCalculate;
					}

					if( toCalculate == 4 ||
						(i == numberOfImpulses - 1 && toCalculate > 0) )
					{
						M::Vector4F		xInputV = M::loadFromMemory( xInput );
						M::Vector4F		yInputV = M::loadFromMemory( yInput );
						M::Vector4F		kV = M::vectorizeOne( this->GetK() );
						M::Vector4F		aV = M::vectorizeOne( this->GetA() );
						M::Vector4F		f0V = M::vectorizeOne( this->GetF0() );
						M::Vector4F		omega0V = M::vectorizeOne( this->GetOmega0() );
						M::Vector4F		wiV = M::loadFromMemory( wiVector );

						M::Vector4F		gaborV = gaborVectorized( kV, aV, f0V, omega0V, xInputV, yInputV );
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

			inline
			ValueType
			cellVectorized( unsigned int* sA, unsigned int* numberOfImpulsesA, ValueType* xA, ValueType* yA ) const
			{
				ValueType	noise = ValueType( 0.0 );

				unsigned int			maxNumberOfImpulses = findMax( numberOfImpulsesA );
				typename M::Vector4I	sV = M::loadFromMemory( (typename M::ScalarI*) sA );
				typename M::Vector4I	numberOfImpulsesV = M::loadFromMemory( (typename M::ScalarI*) numberOfImpulsesA );
				typename M::Vector4F	xV = M::loadFromMemory( xA );
				typename M::Vector4F	yV = M::loadFromMemory( yA );
				typename M::Vector4I	oneIV = M::vectorizeOne( (typename M::ScalarI) 1 );
				typename M::Vector4F	oneFV = M::vectorizeOne( ValueType( 1.0 ) );
				typename M::Vector4F	kernelRadiusV = M::vectorizeOne( this->GetKernelRadius() );

				VECTOR_ALIGN( ValueType		xInputA[ 4 * maxNumberOfImpulses ] );
				VECTOR_ALIGN( ValueType		yInputA[ 4 * maxNumberOfImpulses ] );
				VECTOR_ALIGN( ValueType		wiInputA[ 4 * maxNumberOfImpulses ] );
				VECTOR_ALIGN( unsigned int	calcMaskA[ 4 * maxNumberOfImpulses ] );

				PRNG_VectorType		prngVector( sV );

				for( unsigned int i = 0; i < maxNumberOfImpulses; ++i )
				{
					typename M::Vector4I	impulseMaskV = _mm_cmpgt_epi32( numberOfImpulsesV, _mm_setzero_si128() );
					typename M::Vector4F	xiV = prngVector.uniformNormalized();
					typename M::Vector4F	yiV = prngVector.uniformNormalized();
					typename M::Vector4F	wiV = prngVector.uniformRangeMinusOneToOne();
					typename M::Vector4F	xixV = M::subtract( xV, xiV );
					typename M::Vector4F	yiyV = M::subtract( yV, yiV );

					typename M::Vector4F	radiusXV = M::multiply( xixV, xixV );
					typename M::Vector4F	radiusYV = M::multiply( yiyV, yiyV );
					typename M::Vector4F	radiusV = M::add( radiusXV, radiusYV );
					typename M::Vector4I	radiusMaskV = _mm_cmplt_ps( radiusV, oneFV );
					typename M::Vector4I	calcMaskV = _mm_and_si128( radiusMaskV, impulseMaskV );

					M::storeToMemory( xInputA + (i * 4 ), xixV );
					M::storeToMemory( yInputA + (i * 4 ), yiyV );
					M::storeToMemory( wiInputA + (i * 4 ), wiV );
					M::storeToMemory( (typename M::ScalarI*) calcMaskA + (i * 4 ), calcMaskV );

					numberOfImpulsesV = M::subtract( numberOfImpulsesV, _mm_and_si128( impulseMaskV, oneIV ) );
				}

				int		toCalculate = 0;
				VECTOR_ALIGN( ValueType	xInputPartA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
				VECTOR_ALIGN( ValueType	yInputPartA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
				VECTOR_ALIGN( ValueType	wiInputPartA[ 4 ] ) = { 0.0f, 0.0f, 0.0f, 0.0f };
				for( int g = 0; g < maxNumberOfImpulses * 4; ++g )
				{
					if( calcMaskA[ g ] == 0xffffffff )
					{
						xInputPartA[ toCalculate ] = xInputA[ g ];
						yInputPartA[ toCalculate ] = yInputA[ g ];
						wiInputPartA[ toCalculate ] = wiInputA[ g ];
						++toCalculate;
					}

					if( toCalculate == 4 ||
						(g == maxNumberOfImpulses * 4 - 1 && toCalculate > 0) )
					{
						typename M::Vector4F		xInputV = M::loadFromMemory( xInputPartA );
						typename M::Vector4F		yInputV = M::loadFromMemory( yInputPartA );
						typename M::Vector4F		kV = M::vectorizeOne( this->GetK() );
						typename M::Vector4F		aV = M::vectorizeOne( this->GetA() );
						typename M::Vector4F		f0V = M::vectorizeOne( this->GetF0() );
						typename M::Vector4F		omega0V = M::vectorizeOne( this->GetOmega0() );
						typename M::Vector4F		wiV = M::loadFromMemory( wiInputPartA );

						xInputV = M::multiply( xInputV, kernelRadiusV );
						yInputV = M::multiply( yInputV, kernelRadiusV );

						typename M::Vector4F		gaborV = gaborVectorized( kV, aV, f0V, omega0V, xInputV, yInputV );
						gaborV = M::multiply( wiV, gaborV );

						VECTOR_ALIGN( ValueType	gaborResult[ 4 ] );
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
			M::Vector4I
			mortonVectorized( const M::Vector4I& xV, const M::Vector4I& yV ) const
			{
				M::Vector4I		zV = _mm_setzero_si128();
				M::Vector4I		oneV = M::vectorizeOne( (M::ScalarI) 1 );

				for( unsigned int i = 0; i < (sizeof( unsigned int ) * CHAR_BIT); ++i )
				{
					M::Vector4I		left = _mm_slli_epi32( oneV, i );
					left = _mm_and_si128( xV, left );
					left = _mm_slli_epi32( left, i );

					M::Vector4I		right = _mm_slli_epi32( oneV, i );
					right = _mm_and_si128( right, yV );
					right = _mm_slli_epi32( right, i + 1 );

					M::Vector4I		tmp = _mm_or_si128( left, right );
					zV = _mm_or_si128( zV, tmp );
				}

				/*M::Vector4I		isZero = _mm_cmpeq_epi32( zV, _mm_setzero_si128() );
				M::Vector4I		toOne = _mm_srli_epi32( isZero, 31 );
				zV = M::add( zV, toOne );*/

				return zV;
			}

			inline
			M::Vector4F
			gaborVectorized( M::Vector4F& kV, M::Vector4F& aV, M::Vector4F& f0V, M::Vector4F& omega0V, M::Vector4F& xV, M::Vector4F& yV ) const
			{
				M::Vector4F		piV = M::vectorizeOne( -M::Pi() );
				M::Vector4F		aa = M::multiply( aV, aV );
				M::Vector4F		xx = M::multiply( xV, xV );
				M::Vector4F		yy = M::multiply( yV, yV );
				M::Vector4F		xxyy = M::add( xx, yy );

				M::Vector4F		gaussianEnvelop =  M::multiply( piV, aa );
				gaussianEnvelop = M::multiply( gaussianEnvelop, xxyy );
				gaussianEnvelop = M::exp( gaussianEnvelop );
				gaussianEnvelop = M::multiply( gaussianEnvelop, kV );

				M::Vector4F		sinOmega;
				M::Vector4F		cosOmega;
				M::sinCos( omega0V, sinOmega, cosOmega );
				M::Vector4F		xCosOmega = M::multiply( xV, cosOmega );
				M::Vector4F		ySinOmega = M::multiply( yV, sinOmega );
				M::Vector4F		xCosYSin = M::add( xCosOmega, ySinOmega );

				M::Vector4F		sinusoidalCarrier = M::multiply( M::vectorizeOne( 2.0f ), piV );
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
