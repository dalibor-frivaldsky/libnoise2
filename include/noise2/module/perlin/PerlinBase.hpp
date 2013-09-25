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
#include <noise2/BasicTypes.hpp>
#include <noise2/module/perlin/NoiseGen.hpp>




namespace noise2
{
	
	namespace module
	{

		namespace perlin
		{

			template< typename ValueT >
			class PerlinDefaults;
			



			template<>
			class PerlinDefaults< float >
			{
				public:
				
				/// Default frequency for the noise::module::Perlin noise module.
				static
				float
				Frequency()
				{
					return 1.0f;
				}

				/// Default lacunarity for the noise::module::Perlin noise module.
				static
				float
				Lacunarity()
				{
					return 2.0f;
				}

				/// Default number of octaves for the noise::module::Perlin noise module.
				static
				uint32
				OctaveCount()
				{
					return 6;
				}

				/// Default persistence value for the noise::module::Perlin noise module.
				static
				float
				Persistence()
				{
					return 0.5f;
				}

				/// Default noise quality for the noise::module::Perlin noise module.
				static
				NoiseQuality
				Quality()
				{
					return 

					QUALITY_BEST;
				}

				/// Default noise seed for the noise::module::Perlin noise module.
				static
				uint32
				Seed()
				{
					return 0;
				}

				/// Maximum number of octaves for the noise::module::Perlin noise module.
				static
				uint32
				OctaveCountMax()
				{
					return 30;
				}
			};

			

			template<>
			class PerlinDefaults< double >
			{
				public:
				
				/// Default frequency for the noise::module::Perlin noise module.
				static
				double
				Frequency()
				{
					return 1.0;
				}

				/// Default lacunarity for the noise::module::Perlin noise module.
				static
				double
				Lacunarity()
				{
					return 2.0;
				}

				/// Default number of octaves for the noise::module::Perlin noise module.
				static
				uint32
				OctaveCount()
				{
					return 6;
				}

				/// Default persistence value for the noise::module::Perlin noise module.
				static
				double
				Persistence()
				{
					return 0.5;
				}

				/// Default noise quality for the noise::module::Perlin noise module.
				static
				NoiseQuality
				Quality()
				{
					return QUALITY_BEST;
				}

				/// Default noise seed for the noise::module::Perlin noise module.
				static
				uint32
				Seed()
				{
					return 0;
				}

				/// Maximum number of octaves for the noise::module::Perlin noise module.
				static
				uint32
				OctaveCountMax()
				{
					return 30;
				}
			};

			
		
			template< typename ValueT >
			class PerlinBase
			{
				public:
				
				typedef ValueT							ValueType;
				typedef PerlinDefaults< ValueType >		Defaults;
				
				
				
				public:
				
				PerlinBase():
				  frequency( Defaults::Frequency() ),
				  lacunarity( Defaults::Lacunarity() ),
				  octaveCount( Defaults::OctaveCount() ),
				  persistence( Defaults::Persistence() ),
				  noiseQuality( Defaults::Quality() ),
				  seed( Defaults::Seed() )
				{
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
				}
				
				uint32
				GetOctaveCount() const
				{
					return this->octaveCount;
				}
				
				void
				SetOctaveCount( uint32 octaveCount )
				{
					if( octaveCount < 1 || octaveCount > Defaults::OctaveCountMax() )
					{
						// TODO throw exception
					}
					
					this->octaveCount = octaveCount;
				}
				
				ValueType
				GetPersistence() const
				{
					return this->persistence;
				}
				
				void
				SetPersistence( ValueType persistence )
				{
					this->persistence = persistence;
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
				
				
				
				private:
				
				ValueType			frequency;
				ValueType			lacunarity;
				uint32				octaveCount;
				ValueType			persistence;
				NoiseQuality		noiseQuality;
				uint32				seed;
			};

		}
	
	}
	
}
