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
#include "noise/NoiseGen.hpp"




namespace noise
{
	
	namespace module
	{

		template< typename ValueT >
		class PerlinDefaults;
		
		template<>
		class PerlinDefaults< float >
		{
			public:
			
			/// Default frequency for the noise::module::Perlin noise module.
			static const float Frequency;

			/// Default lacunarity for the noise::module::Perlin noise module.
			static const float Lacunarity;

			/// Default number of octaves for the noise::module::Perlin noise module.
			static const int OctaveCount;

			/// Default persistence value for the noise::module::Perlin noise module.
			static const float Persistence;

			/// Default noise quality for the noise::module::Perlin noise module.
			static const NoiseQuality Quality;

			/// Default noise seed for the noise::module::Perlin noise module.
			static const int Seed;

			/// Maximum number of octaves for the noise::module::Perlin noise module.
			static const int OctaveCountMax;
		};

		const float			PerlinDefaults< float >::Frequency = 1.0f;
		const float			PerlinDefaults< float >::Lacunarity = 2.0f;
		const int			PerlinDefaults< float >::OctaveCount = 6;
		const float			PerlinDefaults< float >::Persistence = 0.5f;
		const NoiseQuality	PerlinDefaults< float >::Quality = QUALITY_STD;
		const int			PerlinDefaults< float >::Seed = 0;
		const int			PerlinDefaults< float >::OctaveCountMax = 30;
		
		template<>
		class PerlinDefaults< double >
		{
			public:
			
			/// Default frequency for the noise::module::Perlin noise module.
			static const double Frequency;

			/// Default lacunarity for the noise::module::Perlin noise module.
			static const double Lacunarity;

			/// Default number of octaves for the noise::module::Perlin noise module.
			static const int OctaveCount;

			/// Default persistence value for the noise::module::Perlin noise module.
			static const double Persistence;

			/// Default noise quality for the noise::module::Perlin noise module.
			static const NoiseQuality Quality;

			/// Default noise seed for the noise::module::Perlin noise module.
			static const int Seed;

			/// Maximum number of octaves for the noise::module::Perlin noise module.
			static const int OctaveCountMax;
		};

		const double		PerlinDefaults< double >::Frequency = 1.0;
		const double		PerlinDefaults< double >::Lacunarity = 2.0;
		const int			PerlinDefaults< double >::OctaveCount = 6;
		const double		PerlinDefaults< double >::Persistence = 0.5;
		const NoiseQuality	PerlinDefaults< double >::Quality = QUALITY_STD;
		const int			PerlinDefaults< double >::Seed = 0;
		const int			PerlinDefaults< double >::OctaveCountMax = 30;
	
	
	
		template< typename ValueT >
		class PerlinBase
		{
			public:
			
			typedef ValueT							ValueType;
			typedef PerlinDefaults< ValueType >		Defaults;
			
			
			
			public:
			
			PerlinBase():
			  frequency( Defaults::Frequency ),
			  lacunarity( Defaults::Lacunarity ),
			  octaveCount( Defaults::OctaveCount ),
			  persistence( Defaults::Persistence ),
			  noiseQuality( Defaults::Quality ),
			  seed( Defaults::Seed )
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
			
			int
			GetOctaveCount() const
			{
				return this->octaveCount;
			}
			
			void
			SetOctaveCount( int octaveCount )
			{
				if( octaveCount < 1 || octaveCount > Defaults::OctaveCountMax )
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
			
			int
			GetSeed() const
			{
				return this->seed;
			}
			
			void
			SetSeed( int seed )
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
			int					octaveCount;
			ValueType			persistence;
			NoiseQuality		noiseQuality;
			int					seed;
		};
	
	}
	
}
