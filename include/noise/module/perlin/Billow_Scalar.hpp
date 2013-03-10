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
#include "noise/math/Math.hpp"
#include "noise/module/Module.hpp"
#include "noise/module/perlin/BillowBase.hpp"
#include "noise/module/perlin/NoiseGen.hpp"




namespace noise
{

	namespace module
	{

		namespace perlin
		{
			
			template< typename ValueT >
			class BillowImpl< ValueT, 1, 1 >: public Module< ValueT, 1 >, public BillowBase< ValueT >
			{
				public:

				typedef ValueT										ValueType;
				static const unsigned								Dimension = 1;
				typedef Module< ValueType, Dimension >				ModuleType;
				typedef BillowBase< ValueType >						BillowBaseType;
				typedef BillowImpl< ValueType, Dimension, 1 >		ThisType;
				typedef math::Math< ValueType >						M;
				typedef NoiseGen< ValueType, Dimension >			Noise;



				public:

				BillowImpl():
				  ModuleType( 0 ),
				  BillowBaseType()
				{}

				virtual
				~BillowImpl()
				{}

				virtual
				ValueType
				GetValue( ValueType x ) const
				{
					ValueType		frequency = this->GetFrequency();
					ValueType		lacunarity = this->GetLacunarity();
					uint32			octaveCount = this->GetOctaveCount();
					ValueType		persistence = this->GetPersistence();
					uint32			seed = this->GetSeed();
					NoiseQuality	noiseQuality = this->GetNoiseQuality();
					
					ValueType	value = 0.0;
					ValueType	signal = 0.0;
					ValueType	curPersistence = 1.0;
					ValueType	nx;
					uint32		octaveSeed;

					x *= frequency;
					
					for( uint32 curOctave = 0; curOctave < octaveCount; curOctave++ )
					{
						// Make sure that these floating-pouint32 values have the same range as a 32-
						// bit uint32eger so that we can pass them to the coherent-noise functions.
						nx = M::MakeInt32Range( x );
						
						// Get the coherent-noise value from the input value and add it to the
						// final result.
						octaveSeed = (seed + curOctave) & 0xffffffff;
						signal = Noise::GradientCoherentNoise( nx, octaveSeed, noiseQuality );
						signal = ValueType( 2.0 ) * fabs( signal ) - ValueType( 1.0 );
						value += signal * curPersistence;

						// Prepare the next octave.
						x *= lacunarity;
						curPersistence *= persistence;
					}
					value += ValueType( 0.5 );

					return value;
				}
			};
			
			
			
			template< typename ValueT >
			class BillowImpl< ValueT, 2, 1 >: public Module< ValueT, 2 >, public BillowBase< ValueT >
			{
				public:

				typedef ValueT										ValueType;
				static const unsigned								Dimension = 2;
				typedef Module< ValueType, Dimension >				ModuleType;
				typedef BillowBase< ValueType >						BillowBaseType;
				typedef BillowImpl< ValueType, Dimension, 1 >		ThisType;
				typedef math::Math< ValueType >						M;
				typedef NoiseGen< ValueType, Dimension >			Noise;



				public:

				BillowImpl():
				  ModuleType( 0 ),
				  BillowBaseType()
				{}

				virtual
				~BillowImpl()
				{}

				virtual
				ValueType
				GetValue( ValueType x, ValueType y ) const
				{
					ValueType		frequency = this->GetFrequency();
					ValueType		lacunarity = this->GetLacunarity();
					uint32			octaveCount = this->GetOctaveCount();
					ValueType		persistence = this->GetPersistence();
					uint32			seed = this->GetSeed();
					NoiseQuality	noiseQuality = this->GetNoiseQuality();
					
					ValueType	value = 0.0;
					ValueType	signal = 0.0;
					ValueType	curPersistence = 1.0;
					ValueType	nx, ny;
					uint32		octaveSeed;

					x *= frequency;
					y *= frequency;
					
					for( uint32 curOctave = 0; curOctave < octaveCount; curOctave++ )
					{
						// Make sure that these floating-pouint32 values have the same range as a 32-
						// bit uint32eger so that we can pass them to the coherent-noise functions.
						nx = M::MakeInt32Range( x );
						ny = M::MakeInt32Range( y );
						
						// Get the coherent-noise value from the input value and add it to the
						// final result.
						octaveSeed = (seed + curOctave) & 0xffffffff;
						signal = Noise::GradientCoherentNoise( nx, ny, octaveSeed, noiseQuality );
						signal = ValueType( 2.0 ) * fabs( signal ) - ValueType( 1.0 );
						value += signal * curPersistence;

						// Prepare the next octave.
						x *= lacunarity;
						y *= lacunarity;
						curPersistence *= persistence;
					}
					value += ValueType( 0.5 );

					return value;
				}
			};

			
			
			template< typename ValueT >
			class BillowImpl< ValueT, 3, 1 >: public Module< ValueT, 3 >, public BillowBase< ValueT >
			{
				public:

				typedef ValueT										ValueType;
				static const unsigned								Dimension = 3;
				typedef Module< ValueType, Dimension >				ModuleType;
				typedef BillowBase< ValueType >						BillowBaseType;
				typedef BillowImpl< ValueType, Dimension, 1 >		ThisType;
				typedef math::Math< ValueType >						M;
				typedef NoiseGen< ValueType, Dimension >			Noise;



				public:

				BillowImpl():
				  ModuleType( 0 ),
				  BillowBaseType()
				{}

				virtual
				~BillowImpl()
				{}

				virtual
				ValueType
				GetValue( ValueType x, ValueType y, ValueType z ) const
				{
					ValueType		frequency = this->GetFrequency();
					ValueType		lacunarity = this->GetLacunarity();
					uint32			octaveCount = this->GetOctaveCount();
					ValueType		persistence = this->GetPersistence();
					uint32			seed = this->GetSeed();
					NoiseQuality	noiseQuality = this->GetNoiseQuality();
					
					ValueType	value = 0.0;
					ValueType	signal = 0.0;
					ValueType	curPersistence = 1.0;
					ValueType	nx, ny, nz;
					uint32		octaveSeed;

					x *= frequency;
					y *= frequency;
					z *= frequency;
					
					for( uint32 curOctave = 0; curOctave < octaveCount; curOctave++ )
					{
						// Make sure that these floating-pouint32 values have the same range as a 32-
						// bit uint32eger so that we can pass them to the coherent-noise functions.
						nx = M::MakeInt32Range( x );
						ny = M::MakeInt32Range( y );
						nz = M::MakeInt32Range( z );
						
						// Get the coherent-noise value from the input value and add it to the
						// final result.
						octaveSeed = (seed + curOctave) & 0xffffffff;
						signal = Noise::GradientCoherentNoise( nx, ny, nz, octaveSeed, noiseQuality );
						signal = ValueType( 2.0 ) * fabs( signal ) - ValueType( 1.0 );
						value += signal * curPersistence;

						// Prepare the next octave.
						x *= lacunarity;
						y *= lacunarity;
						z *= lacunarity;
						curPersistence *= persistence;
					}
					value += ValueType( 0.5 );

					return value;
				}
			};
			
			
			
			template< typename ValueT >
			class BillowImpl< ValueT, 4, 1 >: public Module< ValueT, 4 >, public BillowBase< ValueT >
			{
				public:

				typedef ValueT										ValueType;
				static const unsigned								Dimension = 4;
				typedef Module< ValueType, Dimension >				ModuleType;
				typedef BillowBase< ValueType >						BillowBaseType;
				typedef BillowImpl< ValueType, Dimension, 1 >		ThisType;
				typedef math::Math< ValueType >						M;
				typedef NoiseGen< ValueType, Dimension >			Noise;



				public:

				BillowImpl():
				  ModuleType( 0 ),
				  BillowBaseType()
				{}

				virtual
				~BillowImpl()
				{}

				virtual
				ValueType
				GetValue( ValueType x, ValueType y, ValueType z, ValueType w ) const
				{
					ValueType		frequency = this->GetFrequency();
					ValueType		lacunarity = this->GetLacunarity();
					uint32			octaveCount = this->GetOctaveCount();
					ValueType		persistence = this->GetPersistence();
					uint32			seed = this->GetSeed();
					NoiseQuality	noiseQuality = this->GetNoiseQuality();
					
					ValueType	value = 0.0;
					ValueType	signal = 0.0;
					ValueType	curPersistence = 1.0;
					ValueType	nx, ny, nz, nw;
					uint32		octaveSeed;

					x *= frequency;
					y *= frequency;
					z *= frequency;
					w *= frequency;

					for( uint32 curOctave = 0; curOctave < octaveCount; curOctave++ )
					{
						// Make sure that these floating-pouint32 values have the same range as a 32-
						// bit uint32eger so that we can pass them to the coherent-noise functions.
						nx = M::MakeInt32Range( x );
						ny = M::MakeInt32Range( y );
						nz = M::MakeInt32Range( z );
						nw = M::MakeInt32Range( w );

						// Get the coherent-noise value from the input value and add it to the
						// final result.
						octaveSeed = (seed + curOctave) & 0xffffffff;
						signal = Noise::GradientCoherentNoise( nx, ny, nz, nw, octaveSeed, noiseQuality );
						signal = ValueType( 2.0 ) * fabs( signal ) - ValueType( 1.0 );
						value += signal * curPersistence;

						// Prepare the next octave.
						x *= lacunarity;
						y *= lacunarity;
						z *= lacunarity;
						w *= lacunarity;
						curPersistence *= persistence;
					}
					value += ValueType( 0.5 );

					return value;
				}

			};

		}

	}

}
