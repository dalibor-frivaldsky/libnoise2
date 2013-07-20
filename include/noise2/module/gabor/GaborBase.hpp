#pragma once


// libnoise2
#include <noise2/math/Math.hpp>




namespace noise2
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
				frequencySpread()
				{
					return 0.0f;
				}

				static
				float
				omega0()
				{
					return math::Math< float >::Pi() / 4.0f;
				}

				static
				float
				angularSpread()
				{
					return 0.0f;
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
				frequencySpread()
				{
					return 0.0;
				}

				static
				double
				omega0()
				{
					return math::Math< double >::Pi() / 4.0;
				}

				static
				double
				angularSpread()
				{
					return 0.0;
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
				ValueType		frequencySpread;
				ValueType		omega0;
				ValueType		angularSpread;
				ValueType		numberOfImpulses;
				unsigned int	period;
				unsigned int	seed;

				ValueType		frequencyRangeStart;
				ValueType		frequencyRangeEnd;
				ValueType		angularRangeStart;
				ValueType		angularRangeEnd;

				ValueType		kernelRadius;
				ValueType		impulseDensity;



			public:

				GaborBase():
				  K( Defaults::K() ),
				  a( Defaults::a() ),
				  F0( Defaults::F0() ),
				  frequencySpread( Defaults::frequencySpread() ),
				  omega0( Defaults::omega0() ),
				  angularSpread( Defaults::angularSpread() ),
				  numberOfImpulses( Defaults::numberOfImpulses() ),
				  period( Defaults::period() ),
				  seed( Defaults::seed() ),
				  kernelRadius( ValueType( 0.0 ) ),
				  impulseDensity( ValueType( 0.0 ) )
				{
					recalculateKernelProperties();
					recalculateFrequencyProperties();
					recalculateAngularProperties();
				}

				inline
				ValueType
				GetK() const
				{
					return K;
				}

				inline
				void
				SetK( const ValueType& K )
				{
					this->K = K;
				}

				inline
				ValueType
				GetA() const
				{
					return a;
				}

				inline
				void
				SetA( const ValueType& a )
				{
					this->a = a;
					recalculateKernelProperties();
				}

				inline
				ValueType
				GetF0() const
				{
					return F0;
				}

				inline
				void
				SetF0( const ValueType& F0 )
				{
					this->F0 = F0;
					recalculateFrequencyProperties();
				}

				inline
				ValueType
				GetFrequancySpread() const
				{
					return frequencySpread;
				}

				inline
				void
				SetFrequencySpread( const ValueType& frequencySpread )
				{
					this->frequencySpread = frequencySpread;
					recalculateFrequencyProperties();
				}

				inline
				ValueType
				GetOmega0() const
				{
					return omega0;
				}

				inline
				void
				SetOmega0( const ValueType& omega0 )
				{
					this->omega0 = omega0;
					recalculateAngularProperties();
				}

				inline
				ValueType
				GetAngularSpread() const
				{
					return angularSpread;
				}

				inline
				void
				SetAngularSpread( const ValueType& angularSpread )
				{
					this->angularSpread = angularSpread;
					recalculateAngularProperties();
				}

				inline
				ValueType
				GetNumberOfImpulses() const
				{
					return numberOfImpulses;
				}

				inline
				void
				SetNumberOfImpulses( const ValueType& numberOfImpulses )
				{
					this->numberOfImpulses = numberOfImpulses;
					recalculateKernelProperties();
				}

				// TODO implementd periodic noise
				/*
				inline
				unsigned int
				GetPeriod() const
				{
					return period;
				}

				inline
				void
				SetPeriod( const unsigned int& period )
				{
					this->period = period;
				}
				*/

				inline
				unsigned int
				GetSeed() const
				{
					return seed;
				}

				inline
				void
				SetSeed( const unsigned int& seed )
				{
					this->seed = seed;
				}

				inline
				ValueType
				GetFrequencyRangeStart() const
				{
					return frequencyRangeStart;
				}

				inline
				ValueType
				GetFrequencyRangeEnd() const
				{
					return frequencyRangeEnd;
				}

				inline
				ValueType
				GetAngularRangeStart() const
				{
					return angularRangeStart;
				}

				inline
				ValueType
				GetAngularRangeEnd() const
				{
					return angularRangeEnd;
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
				recalculateKernelProperties()
				{
					kernelRadius = std::sqrt( -std::log( ValueType( 0.05 ) ) / math::Math< ValueType >::Pi() ) / a;
					impulseDensity = numberOfImpulses / (math::Math< ValueType >::Pi() * kernelRadius * kernelRadius);
				}

				void
				recalculateFrequencyProperties()
				{
					frequencyRangeStart = F0 - frequencySpread / ValueType( 2.0 );
					frequencyRangeEnd = F0 + frequencySpread / ValueType( 2.0 );
				}

				void
				recalculateAngularProperties()
				{
					angularRangeStart = omega0 - angularSpread / ValueType( 2.0 );
					angularRangeEnd = omega0 + angularSpread / ValueType( 2.0 );
				}

			};

		}

	}

}
