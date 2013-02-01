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
#include "noise/module/PerlinBase.hpp"




namespace noise
{

	namespace module
	{

		template< typename ValueT, unsigned Dimension >
		class Perlin;
		
		
		
		template< typename ValueT >
		class Perlin< ValueT, 1 >: public Module< ValueT, 1 >, public PerlinBase< ValueT >
		{
			public:

			typedef ValueT								ValueType;
			static const unsigned						Dimension = 1;
			typedef Module< ValueType, Dimension >		ModuleType;
			typedef PerlinBase< ValueType >				PerlinBaseType;
			typedef Perlin< ValueType, Dimension >		ThisType;
			typedef Math< ValueType >					M;
			typedef NoiseGen< ValueType, Dimension >	Noise;



			public:

			Perlin():
			  ModuleType( 0 ),
			  PerlinBaseType()
			{}

			virtual
			~Perlin()
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
		class Perlin< ValueT, 2 >: public Module< ValueT, 2 >, public PerlinBase< ValueT >
		{
			public:

			typedef ValueT								ValueType;
			static const unsigned						Dimension = 2;
			typedef Module< ValueType, Dimension >		ModuleType;
			typedef PerlinBase< ValueType >				PerlinBaseType;
			typedef Perlin< ValueType, Dimension >		ThisType;
			typedef Math< ValueType >					M;
			typedef NoiseGen< ValueType, Dimension >	Noise;



			public:

			Perlin():
			  ModuleType( 0 ),
			  PerlinBaseType()
			{}

			virtual
			~Perlin()
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
		class Perlin< ValueT, 3 >: public Module< ValueT, 3 >, public PerlinBase< ValueT >
		{
			public:

			typedef ValueT								ValueType;
			static const unsigned						Dimension = 3;
			typedef Module< ValueType, Dimension >		ModuleType;
			typedef PerlinBase< ValueType >				PerlinBaseType;
			typedef Perlin< ValueType, Dimension >		ThisType;
			typedef Math< ValueType >					M;
			typedef NoiseGen< ValueType, Dimension >	Noise;



			public:

			Perlin():
			  ModuleType( 0 ),
			  PerlinBaseType()
			{}

			virtual
			~Perlin()
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
		class Perlin< ValueT, 4 >: public Module< ValueT, 4 >, public PerlinBase< ValueT >
		{
			public:

			typedef ValueT								ValueType;
			static const unsigned						Dimension = 4;
			typedef Module< ValueType, Dimension >		ModuleType;
			typedef PerlinBase< ValueType >				PerlinBaseType;
			typedef Perlin< ValueType, Dimension >		ThisType;
			typedef Math< ValueType >					M;
			typedef NoiseGen< ValueType, Dimension >	Noise;



			public:

			Perlin():
			  ModuleType( 0 ),
			  PerlinBaseType()
			{}

			virtual
			~Perlin()
			{}

#if defined( LIBNOISE2_USE_SSE_2 )
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

				typename M::Vector4F		coordV = M::vectorize( x, y, z, w );
				typename M::Vector4F		frequencyV = M::vectorizeOne( frequency );
				typename M::Vector4F		lacunarityV = M::vectorizeOne( lacunarity );

				M::vectorMul( coordV, coordV, frequencyV );

				for( int curOctave = 0; curOctave < octaveCount; curOctave++ ) 
				{
					// Make sure that these floating-point values have the same range as a 32-
					// bit integer so that we can pass them to the coherent-noise functions.
					/*nx = M::MakeInt32Range( x );
					ny = M::MakeInt32Range( y );
					nz = M::MakeInt32Range( z );
					nw = M::MakeInt32Range( w );*/

					// Get the coherent-noise value from the input value and add it to the
					// final result.
					octaveSeed = (seed + curOctave) & 0xffffffff;
					signal = Noise::GradientCoherentNoise( coordV, octaveSeed, noiseQuality );
					value += signal * curPersistence;

					// Prepare the next octave.
					M::vectorMul( coordV, coordV, lacunarityV );
					curPersistence *= persistence;
				}

				return value;
			}
#else
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
#endif

		};

	}

}
