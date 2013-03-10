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
#include "noise/module/simplex/SimplexBase.hpp"
#include "noise/module/simplex/NoiseGen.hpp"




namespace noise
{

	namespace module
	{

		namespace simplex
		{
			
			/*template< typename ValueT >
			class SimplexImpl< ValueT, 1, 1 >: public Module< ValueT, 1 >, public SimplexBase< ValueT >
			{
				public:

				typedef ValueT										ValueType;
				static const unsigned								Dimension = 1;
				typedef Module< ValueType, Dimension >				ModuleType;
				typedef SimplexBase< ValueType >					SimplexBaseType;
				typedef SimplexImpl< ValueType, Dimension, 1 >		ThisType;
				typedef math::Math< ValueType >						M;
				typedef NoiseGen< ValueType, Dimension >			Noise;



				public:

				SimplexImpl():
				  ModuleType( 0 ),
				  SimplexBaseType()
				{}

				virtual
				~SimplexImpl()
				{}

				virtual
				ValueType
				GetValue( ValueType x ) const
				{
					ValueType		frequency = this->GetFrequency();
					ValueType		lacunarity = this->GetLacunarity();
					uint32			octaveCount = this->GetOctaveCount();
					ValueType		persistence = this->GetPersistence();
					
					ValueType	value = 0.0;
					ValueType	signal = 0.0;
					ValueType	curPersistence = 1.0;
					ValueType	nx;

					x *= frequency;
					
					for( uint32 curOctave = 0; curOctave < octaveCount; curOctave++ ) 
					{
						// Make sure that these floating-point values have the same range as a 32-
						// bit integer so that we can pass them to the coherent-noise functions.
						nx = M::MakeInt32Range( x );
						
						// Get the coherent-noise value from the input value and add it to the
						// final result.
						signal = Noise::SimplexNoise( nx );
						value += signal * curPersistence;

						// Prepare the next octave.
						x *= lacunarity;
						curPersistence *= persistence;
					}

					return value;
				}
			};*/
			
			
			
			template< typename ValueT >
			class SimplexImpl< ValueT, 2, 1 >: public Module< ValueT, 2 >, public SimplexBase< ValueT >
			{
				public:

				typedef ValueT										ValueType;
				static const unsigned								Dimension = 2;
				typedef Module< ValueType, Dimension >				ModuleType;
				typedef SimplexBase< ValueType >					SimplexBaseType;
				typedef SimplexImpl< ValueType, Dimension, 1 >		ThisType;
				typedef math::Math< ValueType >						M;
				typedef NoiseGen< ValueType, Dimension >			Noise;



				public:

				SimplexImpl():
				  ModuleType( 0 ),
				  SimplexBaseType()
				{}

				virtual
				~SimplexImpl()
				{}

				virtual
				ValueType
				GetValue( ValueType x, ValueType y ) const
				{
					ValueType		frequency = this->GetFrequency();
					ValueType		lacunarity = this->GetLacunarity();
					uint32			octaveCount = this->GetOctaveCount();
					ValueType		persistence = this->GetPersistence();
					
					ValueType	value = 0.0;
					ValueType	signal = 0.0;
					ValueType	curPersistence = 1.0;
					ValueType	nx, ny;

					x *= frequency;
					y *= frequency;
					
					for( uint32 curOctave = 0; curOctave < octaveCount; curOctave++ ) 
					{
						// Make sure that these floating-point values have the same range as a 32-
						// bit integer so that we can pass them to the coherent-noise functions.
						nx = M::MakeInt32Range( x );
						ny = M::MakeInt32Range( y );
						
						// Get the coherent-noise value from the input value and add it to the
						// final result.
						signal = Noise::SimplexNoise( nx, ny );
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
			class SimplexImpl< ValueT, 3, 1 >: public Module< ValueT, 3 >, public SimplexBase< ValueT >
			{
				public:

				typedef ValueT										ValueType;
				static const unsigned								Dimension = 3;
				typedef Module< ValueType, Dimension >				ModuleType;
				typedef SimplexBase< ValueType >					SimplexBaseType;
				typedef SimplexImpl< ValueType, Dimension, 1 >		ThisType;
				typedef math::Math< ValueType >						M;
				typedef NoiseGen< ValueType, Dimension >			Noise;



				public:

				SimplexImpl():
				  ModuleType( 0 ),
				  SimplexBaseType()
				{}

				virtual
				~SimplexImpl()
				{}

				virtual
				ValueType
				GetValue( ValueType x, ValueType y, ValueType z ) const
				{
					ValueType		frequency = this->GetFrequency();
					ValueType		lacunarity = this->GetLacunarity();
					uint32			octaveCount = this->GetOctaveCount();
					ValueType		persistence = this->GetPersistence();
					
					ValueType	value = 0.0;
					ValueType	signal = 0.0;
					ValueType	curPersistence = 1.0;
					ValueType	nx, ny, nz;

					x *= frequency;
					y *= frequency;
					z *= frequency;
					
					for( uint32 curOctave = 0; curOctave < octaveCount; curOctave++ ) 
					{
						// Make sure that these floating-point values have the same range as a 32-
						// bit integer so that we can pass them to the coherent-noise functions.
						nx = M::MakeInt32Range( x );
						ny = M::MakeInt32Range( y );
						nz = M::MakeInt32Range( z );
						
						// Get the coherent-noise value from the input value and add it to the
						// final result.
						signal = Noise::SimplexNoise( nx, ny, nz );
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
			class SimplexImpl< ValueT, 4, 1 >: public Module< ValueT, 4 >, public SimplexBase< ValueT >
			{
				public:

				typedef ValueT										ValueType;
				static const unsigned								Dimension = 4;
				typedef Module< ValueType, Dimension >				ModuleType;
				typedef SimplexBase< ValueType >					SimplexBaseType;
				typedef SimplexImpl< ValueType, Dimension, 1 >		ThisType;
				typedef math::Math< ValueType >						M;
				typedef NoiseGen< ValueType, Dimension >			Noise;



				public:

				SimplexImpl():
				  ModuleType( 0 ),
				  SimplexBaseType()
				{}

				virtual
				~SimplexImpl()
				{}

				virtual
				ValueType
				GetValue( ValueType x, ValueType y, ValueType z, ValueType w ) const
				{
					ValueType		frequency = this->GetFrequency();
					ValueType		lacunarity = this->GetLacunarity();
					uint32			octaveCount = this->GetOctaveCount();
					ValueType		persistence = this->GetPersistence();
					
					ValueType	value = 0.0;
					ValueType	signal = 0.0;
					ValueType	curPersistence = 1.0;
					ValueType	nx, ny, nz, nw;
					
					x *= frequency;
					y *= frequency;
					z *= frequency;
					w *= frequency;

					for( uint32 curOctave = 0; curOctave < octaveCount; curOctave++ ) 
					{
						// Make sure that these floating-point values have the same range as a 32-
						// bit integer so that we can pass them to the coherent-noise functions.
						nx = M::MakeInt32Range( x );
						ny = M::MakeInt32Range( y );
						nz = M::MakeInt32Range( z );
						nw = M::MakeInt32Range( w );

						// Get the coherent-noise value from the input value and add it to the
						// final result.
						signal = Noise::SimplexNoise( nx, ny, nz, nw );
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
