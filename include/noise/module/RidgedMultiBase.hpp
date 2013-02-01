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
		class RidgedMultiDefaults;
		
		template<>
		class RidgedMultiDefaults< float >
		{
			public:
			
			/// Default frequency for the noise::module::RidgedMulti noise module.
			static const float Frequency;

			/// Default lacunarity for the noise::module::RidgedMulti noise module.
			static const float Lacunarity;

			/// Default number of octaves for the noise::module::RidgedMulti noise module.
			static const int OctaveCount;

			/// Default noise quality for the noise::module::RidgedMulti noise module.
			static const NoiseQuality Quality;

			/// Default noise seed for the noise::module::RidgedMulti noise module.
			static const int Seed;

			/// Maximum number of octaves for the noise::module::RidgedMulti noise module.
			static const int OctaveCountMax;
		};

		const float			RidgedMultiDefaults< float >::Frequency = 1.0f;
		const float			RidgedMultiDefaults< float >::Lacunarity = 2.0f;
		const int			RidgedMultiDefaults< float >::OctaveCount = 6;
		const NoiseQuality	RidgedMultiDefaults< float >::Quality = QUALITY_STD;
		const int			RidgedMultiDefaults< float >::Seed = 0;
		const int			RidgedMultiDefaults< float >::OctaveCountMax = 30;
		
		template<>
		class RidgedMultiDefaults< double >
		{
			public:
			
			/// Default frequency for the noise::module::RidgedMulti noise module.
			static const double Frequency;

			/// Default lacunarity for the noise::module::RidgedMulti noise module.
			static const double Lacunarity;

			/// Default number of octaves for the noise::module::RidgedMulti noise module.
			static const int OctaveCount;

			/// Default noise quality for the noise::module::RidgedMulti noise module.
			static const NoiseQuality Quality;

			/// Default noise seed for the noise::module::RidgedMulti noise module.
			static const int Seed;

			/// Maximum number of octaves for the noise::module::RidgedMulti noise module.
			static const int OctaveCountMax;
		};

		const double		RidgedMultiDefaults< double >::Frequency = 1.0;
		const double		RidgedMultiDefaults< double >::Lacunarity = 2.0;
		const int			RidgedMultiDefaults< double >::OctaveCount = 6;
		const NoiseQuality	RidgedMultiDefaults< double >::Quality = QUALITY_STD;
		const int			RidgedMultiDefaults< double >::Seed = 0;
		const int			RidgedMultiDefaults< double >::OctaveCountMax = 30;
	
	
	
		template< typename ValueT >
		class RidgedMultiBase
		{
			public:
			
			typedef ValueT									ValueType;
			typedef RidgedMultiDefaults< ValueType >		Defaults;
			
			
			
			public:
			
			RidgedMultiBase():
			  frequency( Defaults::Frequency ),
			  lacunarity( Defaults::Lacunarity ),
			  octaveCount( Defaults::OctaveCount ),
			  seed( Defaults::Seed ),
			  noiseQuality( Defaults::Quality )
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
				ValueType	frequency = ValueType( 1.0 );
				
				for( int i = 0; i < Defaults::OctaveCountMax; i++ )
				{
					// Compute weight for each frequency.
					spectralWeights[ i ] = pow( frequency, -h );
					frequency *= lacunarity;
				}
			}
			
			
			
			private:
			
			ValueType			frequency;
			ValueType			lacunarity;
			int					octaveCount;
			int					seed;
			NoiseQuality		noiseQuality;
			ValueType			spectralWeights[ Defaults::OctaveCountMax ];
		};
	
	}
	
}
