#pragma once


// Std C++
#include <vector>


// libnoise2
#include <noise2/Platform.hpp>
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



			public:

				struct Widget
				{

					inline
					Widget():
					  K( Defaults::K() ),
					  a( Defaults::a() ),
					  F0( Defaults::F0() ),
					  frequencySpread( Defaults::frequencySpread() ),
					  omega0( Defaults::omega0() ),
					  angularSpread( Defaults::angularSpread() ),
					  numberOfImpulses( Defaults::numberOfImpulses() ),
					  seed( Defaults::seed() )
					{
					}

					ValueType		K;
					ValueType		a;
					ValueType		F0;
					ValueType		frequencySpread;
					ValueType		omega0;
					ValueType		angularSpread;
					ValueType		numberOfImpulses;
					uint32			seed;

				};



			protected:

				struct PreparedWidget
				{

					inline
					PreparedWidget( const Widget& widget ):
					  K( widget.K ),
					  a( widget.a ),
					  F0( widget.F0 ),
					  frequencySpread( widget.frequencySpread ),
					  omega0( widget.omega0 ),
					  angularSpread( widget.angularSpread ),
					  numberOfImpulses( widget.numberOfImpulses ),
					  seed( widget.seed ),
					  frequencyRangeStart( ValueType( 0.0 ) ),
					  frequencyRangeEnd( ValueType( 0.0 ) ),
					  angularRangeStart( ValueType( 0.0 ) ),
					  angularRangeEnd( ValueType( 0.0 ) ),
					  kernelRadius( ValueType( 0.0 ) ),
					  impulseDensity( ValueType( 0.0 ) ),
					  variance( ValueType( 0.0 ) )
					{
						recalculateKernelProperties();
						recalculateFrequencyProperties();
						recalculateAngularProperties();
						recalculateVariance();
					}


					ValueType		K;
					ValueType		a;
					ValueType		F0;
					ValueType		frequencySpread;
					ValueType		omega0;
					ValueType		angularSpread;
					ValueType		numberOfImpulses;
					uint32			seed;

					ValueType		frequencyRangeStart;
					ValueType		frequencyRangeEnd;
					ValueType		angularRangeStart;
					ValueType		angularRangeEnd;

					ValueType		kernelRadius;
					ValueType		impulseDensity;
					ValueType		variance;



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

					void
					recalculateVariance()
					{
						ValueType	integralGaborFilterSquared = ((K * K) / (ValueType( 4.0 ) * a * a)) * (ValueType( 1.0 ) + math::Math< ValueType >::exp( -(ValueType( 2.0 ) * math::Math< ValueType >::Pi() * F0 * F0) / (a * a) ));
						variance = impulseDensity * (ValueType( 1.0 ) / ValueType( 3.0 )) * integralGaborFilterSquared;
					}

				};

				std::vector< PreparedWidget >		preparedWidgets;
				ValueType							combinedVariance;



			public:

				GaborBase():
				  combinedVariance( 1.0 )
				{
					AddWidget( Widget() );
				}

				inline
				void
				AddWidget( const Widget& widget )
				{
					preparedWidgets.push_back( PreparedWidget( widget ) );
					recalculateCombinedVariance();
				}

				inline
				Widget
				GetWidget( uint32 index ) const
				{
					Widget	widget;

					widget.K = preparedWidgets[ index ].K;
					widget.a = preparedWidgets[ index ].a;
					widget.F0 = preparedWidgets[ index ].F0;
					widget.frequencySpread = preparedWidgets[ index ].frequencySpread;
					widget.omega0 = preparedWidgets[ index ].omega0;
					widget.angularSpread = preparedWidgets[ index ].angularSpread;
					widget.numberOfImpulses = preparedWidgets[ index ].numberOfImpulses;
					widget.seed = preparedWidgets[ index ].seed;

					return widget;
				}

				inline
				void
				SetWidget( uint32 index, const Widget& widget )
				{
					preparedWidgets[ index ] = PreparedWidget( widget );
					recalculateCombinedVariance();
				}

			

			private:

				void
				recalculateCombinedVariance()
				{
					combinedVariance = ValueType( 0.0 );

					for( size_t i = 0; i < preparedWidgets.size(); ++i )
					{
						combinedVariance += preparedWidgets[ i ].variance;
					}
				}

			};
			
		}

	}

}
