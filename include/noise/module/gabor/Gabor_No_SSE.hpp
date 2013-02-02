#pragma once


#include <climits>
#include <cmath>
#include <iostream>


#include <noise/module/Module.hpp>
#include <noise/module/gabor/GaborBase.hpp>
#include <noise/module/gabor/Prng.hpp>




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
			typedef gabor::Prng< ValueType >			PrngType;
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

				for( int di = -1; di <= +1; ++di )
				{
					for( int dj = -1; dj <= +1; ++dj )
					{
						noise += cell( i + di, j + dj, fracX - di, fracY - dj );
					}
				}

				return noise / (ValueType( 3.0 ) * std::sqrt( variance() ));
			}



		private:

			inline
			ValueType
			gabor( float K, float a, float F0, float omega0, float x, float y ) const
			{
				ValueType	gaussianEnvelop = K * std::exp( -M::Pi() * (a * a) * ((x * x) + (y * y)) );
				ValueType	sinusoidalCarrier = std::cos( ValueType( 2.0 ) * M::Pi() * F0 * ((x * std::cos( omega0 )) + (y * std::sin( omega0 ))) );
				return gaussianEnvelop * sinusoidalCarrier;
			}

			inline
			ValueType
			cell( int i, int j, ValueType x, ValueType y ) const
			{
				unsigned int	s = morton( i, j ) + this->GetSeed();

				if( s == 0 )
					s = 1;

				PrngType	prng( s );
				
				ValueType		numberOfImpulsesPerCell = this->GetImpulseDensity() * this->GetKernelRadius() * this->GetKernelRadius();
				unsigned int	numberOfImpulses = prng.poisson( numberOfImpulsesPerCell );
				ValueType		noise = ValueType( 0.0 );

				for( unsigned int i = 0; i < numberOfImpulses; ++i)
				{
					ValueType	xi = prng.uniformNormalized();
					ValueType	yi = prng.uniformNormalized();
					ValueType	wi = prng.uniformRange( -1.0, +1.0 );
					ValueType	F0 = prng.uniformRange( this->GetFrequencyRangeStart(), this->GetFrequencyRangeEnd() );
					ValueType	omega0 = prng.uniformRange( this->GetAngularRangeStart(), this->GetAngularRangeEnd() );
					ValueType	xix = x - xi;
					ValueType	yiy = y - yi;

					if( ((xix * xix) + (yiy * yiy)) < ValueType( 1.0 ) )
					{
						noise += wi * gabor( this->GetK(), this->GetA(), F0, omega0, xix * this->GetKernelRadius(), yiy * this->GetKernelRadius() ); // anisotropic
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

				return z;
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

		};

	}

}
