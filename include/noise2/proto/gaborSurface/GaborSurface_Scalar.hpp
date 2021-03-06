#pragma once


#include <climits>
#include <cmath>
#include <iostream>
#include <bitset>


#include <noise2/math/Math.hpp>
#include <noise2/module/gabor/GaborBase.hpp>
#include <noise2/module/gabor/Prng.hpp>
#include <noise2/proto/gaborSurface/Projection_Scalar.hpp>




namespace noise2
{

	namespace proto
	{

		namespace gaborSurface
		{

			template< typename ValueT >
			class GaborSurfaceImpl< ValueT, 1 >: public module::gabor::GaborBase< ValueT >
			{

			public:

				typedef ValueT									ValueType;
				typedef module::gabor::GaborBase< ValueType >	BaseType;
				typedef GaborSurfaceImpl< ValueType, 1 >		ThisType;



			private:
				typedef module::gabor::Prng< ValueType >	PrngType;
				typedef math::Math< ValueType >				M;



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
					ValueType	noise = ValueType( 0.0 );
					
					for( size_t w = 0; w < this->preparedWidgets.size(); ++w )
					{
						const typename BaseType::PreparedWidget&	widget = this->preparedWidgets[ w ];

						ValueType	widgetPX = pX / widget.kernelRadius;
						ValueType	widgetPY = pY / widget.kernelRadius;
						ValueType	widgetPZ = pZ / widget.kernelRadius;

						ValueType	intX = M::floor( widgetPX );
						ValueType	intY = M::floor( widgetPY );
						ValueType	intZ = M::floor( widgetPZ );
						ValueType	fracX = widgetPX - intX;
						ValueType	fracY = widgetPY - intY;
						ValueType	fracZ = widgetPZ - intZ;
						int32		i = int( intX );
						int32		j = int( intY );
						int32		k = int( intZ );

						for( int32 di = -1; di <= +1; ++di )
						{
							for( int32 dj = -1; dj <= +1; ++dj )
							{
								for( int32 dk = -1; dk <= +1; ++dk )
								{
									noise += cell( widget,
												   i + di, j + dj, k + dk,
												   fracX - di, fracY - dj, fracZ - dk,
												   nX, nY, nZ );
								}
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
				cell( const typename BaseType::PreparedWidget& widget,
					  int32 i, int32 j, int32 k,
					  ValueType pX, ValueType pY, ValueType pZ,
					  ValueType nX, ValueType nY, ValueType nZ ) const
				{
					uint32	s = morton( i, j, k ) + widget.seed;
					
					if( s == 0 )
						s = 1;

					PrngType	prng( s );
					
					ValueType	numberOfImpulsesPerCell = widget.impulseDensity * widget.kernelRadius * widget.kernelRadius / ValueType( 2.0 );
					uint32		numberOfImpulses = prng.poisson( numberOfImpulsesPerCell );
					ValueType	noise = ValueType( 0.0 );

					Projection< ValueType >	projection( nX, nY, nZ,
														ValueType( 1.0 ), ValueType( 0.0 ), ValueType( 0.0 ),
														pX, pY, pZ );

					for( uint32 i = 0; i < numberOfImpulses; ++i)
					{
						ValueType	xi = prng.uniformNormalized();
						ValueType	yi = prng.uniformNormalized();
						ValueType	zi = prng.uniformNormalized();
						ValueType	F0 = prng.uniformRange( widget.frequencyRangeStart, widget.frequencyRangeEnd );
						ValueType	omega0 = prng.uniformRange( widget.angularRangeStart, widget.angularRangeEnd );

						projection.project( xi, yi, zi );
						zi = ValueType( 1.0 ) - fabs( zi );

						if( ((xi * xi) + (yi * yi)) < ValueType( 1.0 ) &&
							zi >= ValueType( 0.0 ) )
						{
							noise += zi * gabor( widget.K, widget.a, F0, omega0, xi * widget.kernelRadius, yi * widget.kernelRadius ); // anisotropic
						}
					}

					return noise;
				}

				inline
				uint32
				morton( uint32 x, uint32 y, uint32 z ) const
				{
					uint32	m = uint32( 0x0 );

					for( uint32 i = 0; i < (2 * CHAR_BIT); ++i )
					{
						m |= ((x & (1 << i)) << (2*i)) | ((y & (1 << i)) << ((2*i) + 1)) | ((z & (1 << i)) << ((2*i) + 2));
					}

					return m;
				}

			};

		}

	}

}
