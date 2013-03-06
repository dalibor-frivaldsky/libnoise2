#pragma once


#include <noise/math/Math.hpp>
#include <noise/module/Module.hpp>
#include <noise/module/perlin/PerlinBase.hpp>
#include <noise/module/perlin/NoiseGen.hpp>




namespace noise
{

	namespace module
	{

		namespace perlin
		{

			template< typename ValueT >
			class PerlinImpl< ValueT, 1 , 1 >: public Module< ValueT, 1 >, public PerlinBase< ValueT >
			{
				public:

				typedef ValueT									ValueType;
				static const unsigned							Dimension = 1;
				typedef Module< ValueType, Dimension >			ModuleType;
				typedef PerlinBase< ValueType >					PerlinBaseType;
				typedef PerlinImpl< ValueType, Dimension, 1 >	ThisType;
				typedef math::Math< ValueType >					M;
				typedef NoiseGen< ValueType, Dimension >		Noise;



				public:

				PerlinImpl
				():
				  ModuleType( 0 ),
				  PerlinBaseType()
				{}

				virtual
				~PerlinImpl()
				{}

				virtual
				ValueType
				GetValue( ValueType x ) const
				{
					ValueType		frequency = this->GetFrequency();
					ValueType		lacunarity = this->GetLacunarity();
					int				octaveCount = this->GetOctaveCount();
					ValueType		persistence = this->GetPersistence();
					int				seed = this->GetSeed();
					NoiseQuality	noiseQuality = this->GetNoiseQuality();
					
					ValueType	value = 0.0;
					ValueType	signal = 0.0;
					ValueType	curPersistence = 1.0;
					ValueType	nx;
					int			octaveSeed;

					x *= frequency;
					
					for( int curOctave = 0; curOctave < octaveCount; curOctave++ ) 
					{
						// Make sure that these floating-point values have the same range as a 32-
						// bit integer so that we can pass them to the coherent-noise functions.
						nx = M::MakeInt32Range( x );
						
						// Get the coherent-noise value from the input value and add it to the
						// final result.
						octaveSeed = (seed + curOctave) & 0xffffffff;
						signal = Noise::GradientCoherentNoise( nx, octaveSeed, noiseQuality );
						value += signal * curPersistence;

						// Prepare the next octave.
						x *= lacunarity;
						curPersistence *= persistence;
					}

					return value;
				}
			};
			
			
			
			template< typename ValueT >
			class PerlinImpl< ValueT, 2, 1 >: public Module< ValueT, 2 >, public PerlinBase< ValueT >
			{
				public:

				typedef ValueT									ValueType;
				static const unsigned							Dimension = 2;
				typedef Module< ValueType, Dimension >			ModuleType;
				typedef PerlinBase< ValueType >					PerlinBaseType;
				typedef PerlinImpl< ValueType, Dimension, 1 >	ThisType;
				typedef math::Math< ValueType >					M;
				typedef NoiseGen< ValueType, Dimension >		Noise;



				public:

				PerlinImpl():
				  ModuleType( 0 ),
				  PerlinBaseType()
				{}

				virtual
				~PerlinImpl()
				{}

				virtual
				ValueType
				GetValue( ValueType x, ValueType y ) const
				{
					ValueType		frequency = this->GetFrequency();
					ValueType		lacunarity = this->GetLacunarity();
					int				octaveCount = this->GetOctaveCount();
					ValueType		persistence = this->GetPersistence();
					int				seed = this->GetSeed();
					NoiseQuality	noiseQuality = this->GetNoiseQuality();
					
					ValueType	value = 0.0;
					ValueType	signal = 0.0;
					ValueType	curPersistence = 1.0;
					ValueType	nx, ny;
					int			octaveSeed;

					x *= frequency;
					y *= frequency;
					
					for( int curOctave = 0; curOctave < octaveCount; curOctave++ ) 
					{
						// Make sure that these floating-point values have the same range as a 32-
						// bit integer so that we can pass them to the coherent-noise functions.
						nx = M::MakeInt32Range( x );
						ny = M::MakeInt32Range( y );
						
						// Get the coherent-noise value from the input value and add it to the
						// final result.
						octaveSeed = (seed + curOctave) & 0xffffffff;
						signal = Noise::GradientCoherentNoise( nx, ny, octaveSeed, noiseQuality );
						value += signal * curPersistence;

						// Prepare the next octave.
						x *= lacunarity;
						y *= lacunarity;
						curPersistence *= persistence;
					}

					return value;
				}
			};

			
			
			template< typename ValueT >
			class PerlinImpl< ValueT, 3, 1 >: public Module< ValueT, 3 >, public PerlinBase< ValueT >
			{
				public:

				typedef ValueT									ValueType;
				static const unsigned							Dimension = 3;
				typedef Module< ValueType, Dimension >			ModuleType;
				typedef PerlinBase< ValueType >					PerlinBaseType;
				typedef PerlinImpl< ValueType, Dimension, 1 >	ThisType;
				typedef math::Math< ValueType >					M;
				typedef NoiseGen< ValueType, Dimension >		Noise;



				public:

				PerlinImpl():
				  ModuleType( 0 ),
				  PerlinBaseType()
				{}

				virtual
				~PerlinImpl()
				{}

				virtual
				ValueType
				GetValue( ValueType x, ValueType y, ValueType z ) const
				{
					ValueType		frequency = this->GetFrequency();
					ValueType		lacunarity = this->GetLacunarity();
					int				octaveCount = this->GetOctaveCount();
					ValueType		persistence = this->GetPersistence();
					int				seed = this->GetSeed();
					NoiseQuality	noiseQuality = this->GetNoiseQuality();
					
					ValueType	value = 0.0;
					ValueType	signal = 0.0;
					ValueType	curPersistence = 1.0;
					ValueType	nx, ny, nz;
					int			octaveSeed;

					x *= frequency;
					y *= frequency;
					z *= frequency;
					
					for( int curOctave = 0; curOctave < octaveCount; curOctave++ ) 
					{
						// Make sure that these floating-point values have the same range as a 32-
						// bit integer so that we can pass them to the coherent-noise functions.
						nx = M::MakeInt32Range( x );
						ny = M::MakeInt32Range( y );
						nz = M::MakeInt32Range( z );
						
						// Get the coherent-noise value from the input value and add it to the
						// final result.
						octaveSeed = (seed + curOctave) & 0xffffffff;
						signal = Noise::GradientCoherentNoise( nx, ny, nz, octaveSeed, noiseQuality );
						value += signal * curPersistence;

						// Prepare the next octave.
						x *= lacunarity;
						y *= lacunarity;
						z *= lacunarity;
						curPersistence *= persistence;
					}

					return value;
				}
			};
			
			
			
			template< typename ValueT >
			class PerlinImpl< ValueT, 4, 1 >: public Module< ValueT, 4 >, public PerlinBase< ValueT >
			{
				public:

				typedef ValueT									ValueType;
				static const unsigned							Dimension = 4;
				typedef Module< ValueType, Dimension >			ModuleType;
				typedef PerlinBase< ValueType >					PerlinBaseType;
				typedef PerlinImpl< ValueType, Dimension, 1 >	ThisType;
				typedef math::Math< ValueType >					M;
				typedef NoiseGen< ValueType, Dimension >		Noise;



				public:

				PerlinImpl():
				  ModuleType( 0 ),
				  PerlinBaseType()
				{}

				virtual
				~PerlinImpl()
				{}

				virtual
				ValueType
				GetValue( ValueType x, ValueType y, ValueType z, ValueType w ) const
				{
					ValueType		frequency = this->GetFrequency();
					ValueType		lacunarity = this->GetLacunarity();
					int				octaveCount = this->GetOctaveCount();
					ValueType		persistence = this->GetPersistence();
					int				seed = this->GetSeed();
					NoiseQuality	noiseQuality = this->GetNoiseQuality();
					
					ValueType	value = 0.0;
					ValueType	signal = 0.0;
					ValueType	curPersistence = 1.0;
					ValueType	nx, ny, nz, nw;
					int			octaveSeed;

					x *= frequency;
					y *= frequency;
					z *= frequency;
					w *= frequency;

					for( int curOctave = 0; curOctave < octaveCount; curOctave++ ) 
					{
						// Make sure that these floating-point values have the same range as a 32-
						// bit integer so that we can pass them to the coherent-noise functions.
						nx = M::MakeInt32Range( x );
						ny = M::MakeInt32Range( y );
						nz = M::MakeInt32Range( z );
						nw = M::MakeInt32Range( w );

						// Get the coherent-noise value from the input value and add it to the
						// final result.
						octaveSeed = (seed + curOctave) & 0xffffffff;
						signal = Noise::GradientCoherentNoise( nx, ny, nz, nw, octaveSeed, noiseQuality );
						value += signal * curPersistence;

						// Prepare the next octave.
						x *= lacunarity;
						y *= lacunarity;
						z *= lacunarity;
						w *= lacunarity;
						curPersistence *= persistence;
					}

					return value;
				}

			};

		}

	}

}
