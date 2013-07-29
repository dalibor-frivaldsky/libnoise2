#pragma once


#include <climits>
#include <cmath>
#include <iostream>


#include <noise2/module/Module.hpp>
#include <noise2/math/Math.hpp>
#include <noise2/module/gabor/GaborBase.hpp>
#include <noise2/module/gabor/Prng.hpp>




namespace noise2
{

	namespace module
	{

		namespace gabor
		{

			template< typename ValueT >
			class GaborImpl< ValueT, 2, 1 >: public Module< ValueT, 2 >, public gabor::GaborBase< ValueT >
			{

			public:

				typedef ValueT									ValueType;
				static const uint32								Dimension = 2;
				typedef Module< ValueType, Dimension >			ModuleType;
				typedef gabor::GaborBase< ValueType >			BaseType;
				typedef GaborImpl< ValueType, Dimension, 1 >	ThisType;



			private:
				typedef gabor::Prng< ValueType >				PrngType;
				typedef math::Math< ValueType >					M;



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
						int32		i = int( intX );
						int32		j = int( intY );

						for( int32 di = -1; di <= 1; ++di )
						{
							for( int32 dj = -1; dj <= 1; ++dj )
							{
								noise += cell( widget, i + di, j + dj, fracX - di, fracY - dj );
							}
						}
					}

					return noise / (ValueType( 3.0 ) * std::sqrt( this->combinedVariance ));
				}



			private:

				inline
				ValueType
				gabor( float K, float a, float F0, float omega0, float x, float y ) const
				{
					ValueType	gaussianEnvelop = K * M::exp( -M::Pi() * (a * a) * ((x * x) + (y * y)) );
					ValueType	omega0Sin;
					ValueType	omega0Cos;
					M::sinCos( omega0, omega0Sin, omega0Cos );
					ValueType	sinusoidalCarrier = M::cos( ValueType( 2.0 ) * M::Pi() * F0 * ((x * omega0Cos) + (y * omega0Sin)) );
					return gaussianEnvelop * sinusoidalCarrier;
				}

				inline
				ValueType
				cell( const typename BaseType::PreparedWidget& widget, int32 i, int32 j, ValueType x, ValueType y ) const
				{
					uint32	s = morton( i, j ) + widget.seed;

					if( s == 0 )
						s = 1;

					PrngType	prng( s );
					
					ValueType	numberOfImpulsesPerCell = widget.impulseDensity * widget.kernelRadius * widget.kernelRadius;
					uint32		numberOfImpulses = prng.poisson( numberOfImpulsesPerCell );
					ValueType	noise = ValueType( 0.0 );

					for( uint32 i = 0; i < numberOfImpulses; ++i)
					{
						ValueType	xi = prng.uniformNormalized();
						ValueType	yi = prng.uniformNormalized();
						ValueType	wi = prng.uniformRange( -1.0, +1.0 );
						ValueType	F0 = prng.uniformRange( widget.frequencyRangeStart, widget.frequencyRangeEnd );
						ValueType	omega0 = prng.uniformRange( widget.angularRangeStart, widget.angularRangeEnd );
						ValueType	xix = x - xi;
						ValueType	yiy = y - yi;

						if( ((xix * xix) + (yiy * yiy)) < ValueType( 1.0 ) )
						{
							noise += wi * gabor( widget.K, widget.a, F0, omega0, xix * widget.kernelRadius, yiy * widget.kernelRadius ); // anisotropic
						}
					}

					return noise;
				}

				inline
				uint32
				morton( uint32 x, uint32 y ) const
				{
					uint32	z = 0;

					for( uint32 i = 0; i < (sizeof( uint32 ) * CHAR_BIT); ++i )
					{
						z |= ((x & (1 << i)) << i) | ((y & (1 << i)) << (i + 1));
					}

					return z;
				}

			};

		}

	}

}
