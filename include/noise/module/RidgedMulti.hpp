// ModuleBase
//
// Copyright (C) 2011 Dalibor Frívaldský
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once


// libnoise
#include "noise/Math.hpp"
#include "noise/NoiseGen.hpp"
#include "noise/module/Module.hpp"
#include "noise/module/RidgedMultiBase.hpp"




namespace noise
{

	namespace module
	{

		template< typename ValueT, unsigned Dimension >
		class RidgedMulti;
		
		
		
		template< typename ValueT >
		class RidgedMulti< ValueT, 1 >: public Module< ValueT, 1 >, public RidgedMultiBase< ValueT >
		{
			public:

			typedef ValueT									ValueType;
			static const unsigned							Dimension = 1;
			typedef Module< ValueType, Dimension >			ModuleType;
			typedef RidgedMultiBase< ValueType >			RidgedMultiBaseType;
			typedef RidgedMulti< ValueType, Dimension >		ThisType;
			typedef Math< ValueType >						M;
			typedef NoiseGen< ValueType, Dimension >		Noise;



			public:

			RidgedMulti():
			  ModuleType( 0 ),
			  RidgedMultiBaseType()
			{}

			virtual
			~RidgedMulti()
			{}

			virtual
			ValueType
			GetValue( ValueType x ) const
			{
				ValueType				frequency = this->GetFrequency();
				ValueType				lacunarity = this->GetLacunarity();
				int						octaveCount = this->GetOctaveCount();
				int						seed = this->GetSeed();
				NoiseQuality			noiseQuality = this->GetNoiseQuality();
				const ValueType*		spectralWeights = this->GetSpectralWeights();
				
				x *= frequency;
				
				ValueType	signal = ValueType( 0.0 );
				ValueType	value  = ValueType( 0.0 );
				ValueType	weight = ValueType( 1.0 );
				int			octaveSeed;

				// These parameters should be user-defined; they may be exposed in a
				// future version of libnoise.
				ValueType	offset = ValueType( 1.0 );
				ValueType	gain = ValueType( 2.0 );

				for( int curOctave = 0; curOctave < octaveCount; curOctave++ )
				{
					// Make sure that these floating-point values have the same range as a 32-
					// bit integer so that we can pass them to the coherent-noise functions.
					ValueType	nx;
					nx = M::MakeInt32Range( x );
					
					// Get the coherent-noise value.
					octaveSeed = (seed + curOctave) & 0x7fffffff;
					signal = Noise::GradientCoherentNoise( nx, octaveSeed, noiseQuality );

					// Make the ridges.
					signal = fabs( signal );
					signal = offset - signal;

					// Square the signal to increase the sharpness of the ridges.
					signal *= signal;

					// The weighting from the previous octave is applied to the signal.
					// Larger values have higher weights, producing sharp points along the
					// ridges.
					signal *= weight;

					// Weight successive contributions by the previous signal.
					weight = signal * gain;
					if( weight > ValueType( 1.0 ) )
					{
						weight = ValueType( 1.0 );
					}
					if( weight < ValueType( 0.0 ) )
					{
						weight = ValueType( 0.0 );
					}

					// Add the signal to the output value.
					value += (signal * spectralWeights[ curOctave ]);

					// Go to the next octave.
					x *= lacunarity;
				}

				return (value * ValueType( 1.25 )) - ValueType( 1.0 );
			}
		};
		
		
		
		template< typename ValueT >
		class RidgedMulti< ValueT, 2 >: public Module< ValueT, 2 >, public RidgedMultiBase< ValueT >
		{
			public:

			typedef ValueT									ValueType;
			static const unsigned							Dimension = 2;
			typedef Module< ValueType, Dimension >			ModuleType;
			typedef RidgedMultiBase< ValueType >			RidgedMultiBaseType;
			typedef RidgedMulti< ValueType, Dimension >		ThisType;
			typedef Math< ValueType >						M;
			typedef NoiseGen< ValueType, Dimension >		Noise;



			public:

			RidgedMulti():
			  ModuleType( 0 ),
			  RidgedMultiBaseType()
			{}

			virtual
			~RidgedMulti()
			{}

			virtual
			ValueType
			GetValue( ValueType x, ValueType y ) const
			{
				ValueType				frequency = this->GetFrequency();
				ValueType				lacunarity = this->GetLacunarity();
				int						octaveCount = this->GetOctaveCount();
				int						seed = this->GetSeed();
				NoiseQuality			noiseQuality = this->GetNoiseQuality();
				const ValueType*		spectralWeights = this->GetSpectralWeights();
				
				x *= frequency;
				y *= frequency;
				
				ValueType	signal = ValueType( 0.0 );
				ValueType	value  = ValueType( 0.0 );
				ValueType	weight = ValueType( 1.0 );
				int			octaveSeed;

				// These parameters should be user-defined; they may be exposed in a
				// future version of libnoise.
				ValueType	offset = ValueType( 1.0 );
				ValueType	gain = ValueType( 2.0 );

				for( int curOctave = 0; curOctave < octaveCount; curOctave++ )
				{
					// Make sure that these floating-point values have the same range as a 32-
					// bit integer so that we can pass them to the coherent-noise functions.
					ValueType	nx, ny;
					nx = M::MakeInt32Range( x );
					ny = M::MakeInt32Range( y );
					
					// Get the coherent-noise value.
					octaveSeed = (seed + curOctave) & 0x7fffffff;
					signal = Noise::GradientCoherentNoise( nx, ny, octaveSeed, noiseQuality );

					// Make the ridges.
					signal = fabs( signal );
					signal = offset - signal;

					// Square the signal to increase the sharpness of the ridges.
					signal *= signal;

					// The weighting from the previous octave is applied to the signal.
					// Larger values have higher weights, producing sharp points along the
					// ridges.
					signal *= weight;

					// Weight successive contributions by the previous signal.
					weight = signal * gain;
					if( weight > ValueType( 1.0 ) )
					{
						weight = ValueType( 1.0 );
					}
					if( weight < ValueType( 0.0 ) )
					{
						weight = ValueType( 0.0 );
					}

					// Add the signal to the output value.
					value += (signal * spectralWeights[ curOctave ]);

					// Go to the next octave.
					x *= lacunarity;
					y *= lacunarity;
				}

				return (value * ValueType( 1.25 )) - ValueType( 1.0 );
			}
		};

		
		
		template< typename ValueT >
		class RidgedMulti< ValueT, 3 >: public Module< ValueT, 3 >, public RidgedMultiBase< ValueT >
		{
			public:

			typedef ValueT									ValueType;
			static const unsigned							Dimension = 3;
			typedef Module< ValueType, Dimension >			ModuleType;
			typedef RidgedMultiBase< ValueType >			RidgedMultiBaseType;
			typedef RidgedMulti< ValueType, Dimension >		ThisType;
			typedef Math< ValueType >						M;
			typedef NoiseGen< ValueType, Dimension >		Noise;



			public:

			RidgedMulti():
			  ModuleType( 0 ),
			  RidgedMultiBaseType()
			{}

			virtual
			~RidgedMulti()
			{}

			virtual
			ValueType
			GetValue( ValueType x, ValueType y, ValueType z ) const
			{
				ValueType				frequency = this->GetFrequency();
				ValueType				lacunarity = this->GetLacunarity();
				int						octaveCount = this->GetOctaveCount();
				int						seed = this->GetSeed();
				NoiseQuality			noiseQuality = this->GetNoiseQuality();
				const ValueType*		spectralWeights = this->GetSpectralWeights();
				
				x *= frequency;
				y *= frequency;
				z *= frequency;
				
				ValueType	signal = ValueType( 0.0 );
				ValueType	value  = ValueType( 0.0 );
				ValueType	weight = ValueType( 1.0 );
				int			octaveSeed;

				// These parameters should be user-defined; they may be exposed in a
				// future version of libnoise.
				ValueType	offset = ValueType( 1.0 );
				ValueType	gain = ValueType( 2.0 );

				for( int curOctave = 0; curOctave < octaveCount; curOctave++ )
				{
					// Make sure that these floating-point values have the same range as a 32-
					// bit integer so that we can pass them to the coherent-noise functions.
					ValueType	nx, ny, nz;
					nx = M::MakeInt32Range( x );
					ny = M::MakeInt32Range( y );
					nz = M::MakeInt32Range( z );
					
					// Get the coherent-noise value.
					octaveSeed = (seed + curOctave) & 0x7fffffff;
					signal = Noise::GradientCoherentNoise( nx, ny, nz, octaveSeed, noiseQuality );

					// Make the ridges.
					signal = fabs( signal );
					signal = offset - signal;

					// Square the signal to increase the sharpness of the ridges.
					signal *= signal;

					// The weighting from the previous octave is applied to the signal.
					// Larger values have higher weights, producing sharp points along the
					// ridges.
					signal *= weight;

					// Weight successive contributions by the previous signal.
					weight = signal * gain;
					if( weight > ValueType( 1.0 ) )
					{
						weight = ValueType( 1.0 );
					}
					if( weight < ValueType( 0.0 ) )
					{
						weight = ValueType( 0.0 );
					}

					// Add the signal to the output value.
					value += (signal * spectralWeights[ curOctave ]);

					// Go to the next octave.
					x *= lacunarity;
					y *= lacunarity;
					z *= lacunarity;
				}

				return (value * ValueType( 1.25 )) - ValueType( 1.0 );
			}
		};
		
		
		
		template< typename ValueT >
		class RidgedMulti< ValueT, 4 >: public Module< ValueT, 4 >, public RidgedMultiBase< ValueT >
		{
			public:

			typedef ValueT									ValueType;
			static const unsigned							Dimension = 4;
			typedef Module< ValueType, Dimension >			ModuleType;
			typedef RidgedMultiBase< ValueType >			RidgedMultiBaseType;
			typedef RidgedMulti< ValueType, Dimension >		ThisType;
			typedef Math< ValueType >						M;
			typedef NoiseGen< ValueType, Dimension >		Noise;



			public:

			RidgedMulti():
			  ModuleType( 0 ),
			  RidgedMultiBaseType()
			{}

			virtual
			~RidgedMulti()
			{}

#if defined( LIBNOISE2_USE_SSE_2 )
			virtual
			ValueType
			GetValue( ValueType x, ValueType y, ValueType z, ValueType w ) const
			{
				ValueType				frequency = this->GetFrequency();
				ValueType				lacunarity = this->GetLacunarity();
				int						octaveCount = this->GetOctaveCount();
				int						seed = this->GetSeed();
				NoiseQuality			noiseQuality = this->GetNoiseQuality();
				const ValueType*		spectralWeights = this->GetSpectralWeights();
				
				x *= frequency;
				y *= frequency;
				z *= frequency;
				w *= frequency;

				ValueType	signal = ValueType( 0.0 );
				ValueType	value  = ValueType( 0.0 );
				ValueType	weight = ValueType( 1.0 );
				int			octaveSeed;

				// These parameters should be user-defined; they may be exposed in a
				// future version of libnoise.
				ValueType	offset = ValueType( 1.0 );
				ValueType	gain = ValueType( 2.0 );

				for( int curOctave = 0; curOctave < octaveCount; curOctave++ )
				{
					// Make sure that these floating-point values have the same range as a 32-
					// bit integer so that we can pass them to the coherent-noise functions.
					ValueType	nx, ny, nz, nw;
					nx = M::MakeInt32Range( x );
					ny = M::MakeInt32Range( y );
					nz = M::MakeInt32Range( z );
					nw = M::MakeInt32Range( w );

					// Get the coherent-noise value.
					octaveSeed = (seed + curOctave) & 0x7fffffff;
					typename M::Vector4F		coordV = M::vectorize( nx, ny, nz, nw );
					signal = Noise::GradientCoherentNoise( coordV, octaveSeed, noiseQuality );

					// Make the ridges.
					signal = fabs( signal );
					signal = offset - signal;

					// Square the signal to increase the sharpness of the ridges.
					signal *= signal;

					// The weighting from the previous octave is applied to the signal.
					// Larger values have higher weights, producing sharp points along the
					// ridges.
					signal *= weight;

					// Weight successive contributions by the previous signal.
					weight = signal * gain;
					if( weight > ValueType( 1.0 ) )
					{
						weight = ValueType( 1.0 );
					}
					if( weight < ValueType( 0.0 ) )
					{
						weight = ValueType( 0.0 );
					}

					// Add the signal to the output value.
					value += (signal * spectralWeights[ curOctave ]);

					// Go to the next octave.
					x *= lacunarity;
					y *= lacunarity;
					z *= lacunarity;
					w *= lacunarity;
				}

				return (value * ValueType( 1.25 )) - ValueType( 1.0 );
			}
#else
			virtual
			ValueType
			GetValue( ValueType x, ValueType y, ValueType z, ValueType w ) const
			{
				ValueType				frequency = this->GetFrequency();
				ValueType				lacunarity = this->GetLacunarity();
				int						octaveCount = this->GetOctaveCount();
				int						seed = this->GetSeed();
				NoiseQuality			noiseQuality = this->GetNoiseQuality();
				const ValueType*		spectralWeights = this->GetSpectralWeights();
				
				x *= frequency;
				y *= frequency;
				z *= frequency;
				w *= frequency;

				ValueType	signal = ValueType( 0.0 );
				ValueType	value  = ValueType( 0.0 );
				ValueType	weight = ValueType( 1.0 );
				int			octaveSeed;

				// These parameters should be user-defined; they may be exposed in a
				// future version of libnoise.
				ValueType	offset = ValueType( 1.0 );
				ValueType	gain = ValueType( 2.0 );

				for( int curOctave = 0; curOctave < octaveCount; curOctave++ )
				{
					// Make sure that these floating-point values have the same range as a 32-
					// bit integer so that we can pass them to the coherent-noise functions.
					ValueType	nx, ny, nz, nw;
					nx = M::MakeInt32Range( x );
					ny = M::MakeInt32Range( y );
					nz = M::MakeInt32Range( z );
					nw = M::MakeInt32Range( w );

					// Get the coherent-noise value.
					octaveSeed = (seed + curOctave) & 0x7fffffff;
					signal = Noise::GradientCoherentNoise( nx, ny, nz, nw, octaveSeed, noiseQuality );

					// Make the ridges.
					signal = fabs( signal );
					signal = offset - signal;

					// Square the signal to increase the sharpness of the ridges.
					signal *= signal;

					// The weighting from the previous octave is applied to the signal.
					// Larger values have higher weights, producing sharp points along the
					// ridges.
					signal *= weight;

					// Weight successive contributions by the previous signal.
					weight = signal * gain;
					if( weight > ValueType( 1.0 ) )
					{
						weight = ValueType( 1.0 );
					}
					if( weight < ValueType( 0.0 ) )
					{
						weight = ValueType( 0.0 );
					}

					// Add the signal to the output value.
					value += (signal * spectralWeights[ curOctave ]);

					// Go to the next octave.
					x *= lacunarity;
					y *= lacunarity;
					z *= lacunarity;
					w *= lacunarity;
				}

				return (value * ValueType( 1.25 )) - ValueType( 1.0 );
			}
#endif
		};

	}

}
