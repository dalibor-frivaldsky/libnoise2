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




namespace noise
{
	
	namespace module
	{

		template< typename ValueT >
		class SimplexDefaults;
		
		template<>
		class SimplexDefaults< float >
		{
			public:
			
			/// Default frequency for the noise::module::Simplex noise module.
			static const float Frequency;

			/// Default lacunarity for the noise::module::Simplex noise module.
			static const float Lacunarity;

			/// Default number of octaves for the noise::module::Simplex noise module.
			static const int OctaveCount;

			/// Default persistence value for the noise::module::Simplex noise module.
			static const float Persistence;

			/// Maximum number of octaves for the noise::module::Simplex noise module.
			static const int OctaveCountMax;
		};

		const float			SimplexDefaults< float >::Frequency = 1.0f;
		const float			SimplexDefaults< float >::Lacunarity = 2.0f;
		const int			SimplexDefaults< float >::OctaveCount = 6;
		const float			SimplexDefaults< float >::Persistence = 0.5f;
		const int			SimplexDefaults< float >::OctaveCountMax = 30;
		
		template<>
		class SimplexDefaults< double >
		{
			public:
			
			/// Default frequency for the noise::module::Simplex noise module.
			static const double Frequency;

			/// Default lacunarity for the noise::module::Simplex noise module.
			static const double Lacunarity;

			/// Default number of octaves for the noise::module::Simplex noise module.
			static const int OctaveCount;

			/// Default persistence value for the noise::module::Simplex noise module.
			static const double Persistence;

			/// Maximum number of octaves for the noise::module::Simplex noise module.
			static const int OctaveCountMax;
		};

		const double		SimplexDefaults< double >::Frequency = 1.0;
		const double		SimplexDefaults< double >::Lacunarity = 2.0;
		const int			SimplexDefaults< double >::OctaveCount = 6;
		const double		SimplexDefaults< double >::Persistence = 0.5;
		const int			SimplexDefaults< double >::OctaveCountMax = 30;
	
	
	
		template< typename ValueT >
		class SimplexBase
		{
			public:
			
			typedef ValueT								ValueType;
			typedef SimplexDefaults< ValueType >		Defaults;
			
			
			
			public:
			
			SimplexBase():
			  frequency( Defaults::Frequency ),
			  lacunarity( Defaults::Lacunarity ),
			  octaveCount( Defaults::OctaveCount ),
			  persistence( Defaults::Persistence )
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
			

			
			private:
			
			ValueType			frequency;
			ValueType			lacunarity;
			int					octaveCount;
			ValueType			persistence;
		};
	
	}
	
}
