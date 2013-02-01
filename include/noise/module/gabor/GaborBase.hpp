#pragma once


// libnoise2
#include <noise/Math.hpp>




namespace noise
{

	namespace module
	{

		namespace gabor
		{

			template< typename ValueT >
			class GaborDefaults;

			template<>
			class GaborDefaults< float >
			{

			public:

				static
				float
				K()
				{
					return 1.0f;
				}

				static
				float
				a()
				{
					return 0.05f;
				}

				static
				float
				F0()
				{
					return 0.0625f;
				}

				static
				float
				omega0()
				{
					return Math< float >::Pi() / 4.0f;
				}

				static
				float
				numberOfImpulses()
				{
					return 64.0f;
				}

				static
				unsigned int
				period()
				{
					return 256;
				}

				static
				unsigned int
				seed()
				{
					return 0;
				}

			};

			template<>
			class GaborDefaults< double >
			{

			public:

				static
				double
				K()
				{
					return 1.0;
				}

				static
				double
				a()
				{
					return 0.05;
				}

				static
				double
				F0()
				{
					return 0.0625;
				}

				static
				double
				omega0()
				{
					return Math< double >::Pi() / 4.0;
				}

				static
				double
				numberOfImpulses()
				{
					return 64.0;
				}

				static
				unsigned int
				period()
				{
					return 256;
				}

				static
				unsigned int
				seed()
				{
					return 0;
				}

			};



			template< typename ValueT >
			class GaborBase
			{

			public:

				typedef ValueT							ValueType;
				typedef GaborDefaults< ValueType >		Defaults;



			protected:

				ValueType		K;
				ValueType		a;
				ValueType		F0;
				ValueType		omega0;
				ValueType		numberOfImpulses;
				unsigned int	period;
				unsigned int	seed;

				ValueType		kernelRadius;
				ValueType		impulseDensity;



			public:

				GaborBase():
				  K( Defaults::K() ),
				  a( Defaults::a() ),
				  F0( Defaults::F0() ),
				  omega0( Defaults::omega0() ),
				  numberOfImpulses( Defaults::numberOfImpulses() ),
				  period( Defaults::period() ),
				  seed( Defaults::seed() ),
				  kernelRadius( ValueType( 0.0 ) ),
				  impulseDensity( ValueType( 0.0 ) )
				{
					recalculateProperties();
				}

				inline
				ValueType
				GetK() const
				{
					return K;
				}

				inline
				ValueType
				GetA() const
				{
					return a;
				}

				inline
				ValueType
				GetF0() const
				{
					return F0;
				}

				inline
				ValueType
				GetOmega0() const
				{
					return omega0;
				}

				inline
				ValueType
				GetNumberOfImpulses() const
				{
					return numberOfImpulses;
				}

				inline
				unsigned int
				GetPeriod() const
				{
					return period;
				}

				inline
				unsigned int
				GetSeed() const
				{
					return seed;
				}

				inline
				ValueType
				GetKernelRadius() const
				{
					return kernelRadius;
				}

				inline
				ValueType
				GetImpulseDensity() const
				{
					return impulseDensity;
				}



			private:

				void
				recalculateProperties()
				{
					kernelRadius = std::sqrt( -std::log( ValueType( 0.05 ) ) / Math< ValueType >::Pi() ) / a;
					impulseDensity = numberOfImpulses / (Math< ValueType >::Pi() * kernelRadius * kernelRadius);
				}

			};

		}

	}

}
