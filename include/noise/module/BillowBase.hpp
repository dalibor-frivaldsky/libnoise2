// ModuleBase
//
// Copyright (C) 2011 Dalibor Fr�valdsk�
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
		class BillowDefaults;
		
		template<>
		class BillowDefaults< float >
		{
			public:
			
			/// Default frequency for the noise::module::Billow noise module.
			static const float Frequency;

			/// Default lacunarity for the noise::module::Billow noise module.
			static const float Lacunarity;

			/// Default number of octaves for the noise::module::Billow noise module.
			static const int OctaveCount;

			/// Default persistence value for the noise::module::Billow noise module.
			static const float Persistence;

			/// Default noise quality for the noise::module::Billow noise module.
			static const NoiseQuality Quality;

			/// Default noise seed for the noise::module::Billow noise module.
			static const int Seed;

			/// Maximum number of octaves for the noise::module::Billow noise module.
			static const int OctaveCountMax;
		};

		const float			BillowDefaults< float >::Frequency = 1.0f;
		const float			BillowDefaults< float >::Lacunarity = 2.0f;
		const int			BillowDefaults< float >::OctaveCount = 6;
		const float			BillowDefaults< float >::Persistence = 0.5f;
		const NoiseQuality	BillowDefaults< float >::Quality = QUALITY_STD;
		const int			BillowDefaults< float >::Seed = 0;
		const int			BillowDefaults< float >::OctaveCountMax = 30;
		
		template<>
		class BillowDefaults< double >
		{
			public:
			
			/// Default frequency for the noise::module::Billow noise module.
			static const double Frequency;

			/// Default lacunarity for the noise::module::Billow noise module.
			static const double Lacunarity;

			/// Default number of octaves for the noise::module::Billow noise module.
			static const int OctaveCount;

			/// Default persistence value for the noise::module::Billow noise module.
			static const double Persistence;

			/// Default noise quality for the noise::module::Billow noise module.
			static const NoiseQuality Quality;

			/// Default noise seed for the noise::module::Billow noise module.
			static const int Seed;

			/// Maximum number of octaves for the noise::module::Billow noise module.
			static const int OctaveCountMax;
		};

		const double		BillowDefaults< double >::Frequency = 1.0;
		const double		BillowDefaults< double >::Lacunarity = 2.0;
		const int			BillowDefaults< double >::OctaveCount = 6;
		const double		BillowDefaults< double >::Persistence = 0.5;
		const NoiseQuality	BillowDefaults< double >::Quality = QUALITY_STD;
		const int			BillowDefaults< double >::Seed = 0;
		const int			BillowDefaults< double >::OctaveCountMax = 30;
	
	
	
		template< typename ValueT >
		class BillowBase
		{
			public:
			
			typedef ValueT							ValueType;
			typedef BillowDefaults< ValueType >		Defaults;
			
			
			
			public:
			
			BillowBase():
			  frequency( Defaults::Frequency ),
			  lacunarity( Defaults::Lacunarity ),
			  octaveCount( Defaults::OctaveCount ),
			  persistence( Defaults::Persistence ),
			  seed( Defaults::Seed ),
			  noiseQuality( Defaults::Quality )
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
			int					seed;
			NoiseQuality		noiseQuality;
		};
	
	}
	
}
