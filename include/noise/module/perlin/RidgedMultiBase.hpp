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
#include <noise/BasicTypes.hpp>
#include <noise/module/perlin/NoiseGen.hpp>




namespace noise
{
	
	namespace module
	{

		namespace perlin
		{
			
			template< typename ValueT >
			class RidgedMultiDefaults;
			
			template<>
			class RidgedMultiDefaults< float >
			{
				public:
				
				/// Default frequency for the noise::module::Billow noise module.
				static inline
				float
				Frequency()
				{
					return 1.0f;
				}

				/// Default lacunarity for the noise::module::Billow noise module.
				static inline
				float
				Lacunarity()
				{
					return 2.0f;
				}
				
				/// Default number of octaves for the noise::module::Billow noise module.
				static inline
				uint32
				OctaveCount()
				{
					return 6;
				}
				
				/// Default noise quality for the noise::module::Billow noise module.
				static inline
				NoiseQuality
				Quality()
				{
					return QUALITY_STD;
				}
				
				/// Default noise seed for the noise::module::Billow noise module.
				static inline
				uint32
				Seed()
				{
					return 0;
				}
				
				/// Maximum number of octaves for the noise::module::Billow noise module.
				static inline
				uint32
				OctaveCountMax()
				{
					return 30;
				}
			};

			template<>
			class RidgedMultiDefaults< double >
			{

				public:
				
				/// Default frequency for the noise::module::Billow noise module.
				static inline
				double
				Frequency()
				{
					return 1.0;
				}

				/// Default lacunarity for the noise::module::Billow noise module.
				static inline
				double
				Lacunarity()
				{
					return 2.0;
				}
				
				/// Default number of octaves for the noise::module::Billow noise module.
				static inline
				uint32
				OctaveCount()
				{
					return 6;
				}
				
				/// Default noise quality for the noise::module::Billow noise module.
				static inline
				NoiseQuality
				Quality()
				{
					return QUALITY_STD;
				}
				
				/// Default noise seed for the noise::module::Billow noise module.
				static inline
				uint32
				Seed()
				{
					return 0;
				}
				
				/// Maximum number of octaves for the noise::module::Billow noise module.
				static inline
				uint32
				OctaveCountMax()
				{
					return 30;
				}

			};

			
		
			template< typename ValueT >
			class RidgedMultiBase
			{
				public:
				
				typedef ValueT									ValueType;
				typedef RidgedMultiDefaults< ValueType >		Defaults;
				
				
				
				public:
				
				RidgedMultiBase():
				  frequency( Defaults::Frequency() ),
				  lacunarity( Defaults::Lacunarity() ),
				  octaveCount( Defaults::OctaveCount() ),
				  seed( Defaults::Seed() ),
				  noiseQuality( Defaults::Quality() )
				{
					CalculateSpectralWeights();
				}
				
				ValueType
				GetFrequency() const
				{
					return this->frequency;
				}
				
				void
				SetFrequency( ValueType frequency )
				{
					this->frequency = frequency;

					CalculateSpectralWeights();
				}
				
				ValueType
				GetLacunarity() const
				{
					return this->lacunarity;
				}
				
				void
				SetLacunarity( ValueType lacunarity )
				{
					this->lacunarity = lacunarity;
					
					CalculateSpectralWeights();
				}
				
				uint32
				GetOctaveCount() const
				{
					return this->octaveCount;
				}
				
				void
				SetOctaveCount( uint32 octaveCount )
				{
					if( octaveCount < 1 || octaveCount > Defaults::OctaveCountMax )
					{
						// TODO throw exception
					}
					
					this->octaveCount = octaveCount;
				}
				
				uint32
				GetSeed() const
				{
					return this->seed;
				}
				
				void
				SetSeed( uint32 seed )
				{
					this->seed = seed;
				}
				
				NoiseQuality
				GetNoiseQuality() const
				{
					return this->noiseQuality;
				}
				
				void
				SetNoiseQuality( NoiseQuality noiseQuality )
				{
					this->noiseQuality = noiseQuality;
				}
				
				
				
				protected:
				
				const ValueType*
				GetSpectralWeights() const
				{
					return spectralWeights;
				}
				
				
				
				private:
				
				void
				CalculateSpectralWeights()
				{
					// This exponent parameter should be user-defined; it may be exposed in a
					// future version of libnoise.
					ValueType	h = ValueType( 1.0 );
					ValueType	frequency = this->frequency;
					
					for( uint32 i = 0; i < Defaults::OctaveCountMax(); i++ )
					{
						// Compute weight for each frequency.
						spectralWeights[ i ] = pow( frequency, -h );
						frequency *= lacunarity;
					}
				}
				
				
				
				private:
				
				ValueType			frequency;
				ValueType			lacunarity;
				uint32				octaveCount;
				uint32				seed;
				NoiseQuality		noiseQuality;
				ValueType			spectralWeights[ 30 ];
			};

		}
	
	}
	
}
