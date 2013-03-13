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
			class BillowImpl< ValueT, 1, 4 >: public Module< ValueT, 1 >, public BillowBase< ValueT >
			{
				public:

				typedef ValueT										ValueType;
				static const unsigned								Dimension = 1;
				typedef Module< ValueType, Dimension >				ModuleType;
				typedef BillowBase< ValueType >						BillowBaseType;
				typedef BillowImpl< ValueType, Dimension, 4 >		ThisType;
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

				void
				GetValue4( const ValueType* inputX, ValueType* output )
				{
					typename M::Vector4F	xV = M::loadFromMemory( inputX );

					typename M::Vector4F	frequencyV = M::vectorizeOne( this->GetFrequency() );
					typename M::Vector4F	lacunarityV = M::vectorizeOne( this->GetLacunarity() );
					uint32					octaveCount = this->GetOctaveCount();
					typename M::Vector4F	persistenceV = M::vectorizeOne( this->GetPersistence() );
					typename M::Vector4I	seedV = M::vectorizeOne( this->GetSeed() );
					NoiseQuality			noiseQuality = this->GetNoiseQuality();

					typename M::Vector4F	valueV = M::constZeroF();
					typename M::Vector4F	signalV = M::constZeroF();
					typename M::Vector4F	curPersistenceV = M::constOneF();
					typename M::Vector4F	nxV;
					
					for( uint32 curOctave = 0; curOctave < octaveCount; curOctave++ ) 
					{
						// Make sure that these floating-point values have the same range as a 32-
						// bit integer so that we can pass them to the coherent-noise functions.
						//nx = M::MakeInt32Range( x );
						nxV = xV;
						
						// Get the coherent-noise value from the input value and add it to the
						// final result.
						signalV = Noise::GradientCoherentNoise4( nxV, seedV, noiseQuality );
						signalV = M::subtract( M::multiply( M::constTwoF(), M::abs( signalV ) ), M::constOneF() );
						valueV = M::add( valueV, M::multiply( signalV, curPersistenceV ) );

						// Prepare the next octave.
						xV = M::multiply( xV, lacunarityV );
						curPersistenceV = M::multiply( curPersistenceV, persistenceV );
						seedV = M::add( seedV, M::constOneI() );
					}

					valueV = M::add( valueV, M::vectorizeOne( ValueType( 0.5 ) ) );
					M::storeToMemory( output, valueV );
				}

			};
			
			
			
			template< typename ValueT >
			class BillowImpl< ValueT, 2, 4 >: public Module< ValueT, 2 >, public BillowBase< ValueT >
			{
				public:

				typedef ValueT										ValueType;
				static const unsigned								Dimension = 2;
				typedef Module< ValueType, Dimension >				ModuleType;
				typedef BillowBase< ValueType >						BillowBaseType;
				typedef BillowImpl< ValueType, Dimension, 4 >		ThisType;
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

					typename M::Vector4F		coordV = M::vectorize( x, y );
					typename M::Vector4F		frequencyV = M::vectorizeOne( frequency );
					typename M::Vector4F		lacunarityV = M::vectorizeOne( lacunarity );

					coordV = M::multiply( coordV, frequencyV );
					
					for( uint32 curOctave = 0; curOctave < octaveCount; curOctave++ )
					{
						// Make sure that these floating-pouint32 values have the same range as a 32-
						// bit uint32eger so that we can pass them to the coherent-noise functions.
						/*nx = M::MakeInt32Range( x );
						ny = M::MakeInt32Range( y );*/
						
						// Get the coherent-noise value from the input value and add it to the
						// final result.
						octaveSeed = (seed + curOctave) & 0xffffffff;
						signal = Noise::GradientCoherentNoise( coordV, octaveSeed, noiseQuality );
						signal = ValueType( 2.0 ) * fabs( signal ) - ValueType( 1.0 );
						value += signal * curPersistence;

						// Prepare the next octave.
						coordV = M::multiply( coordV, lacunarityV );
						curPersistence *= persistence;
					}
					value += ValueType( 0.5 );

					return value;
				}

				void
				GetValue4( const ValueType* inputX, const ValueType* inputY, ValueType* output )
				{
					typename M::Vector4F	xV = M::loadFromMemory( inputX );
					typename M::Vector4F	yV = M::loadFromMemory( inputY );

					typename M::Vector4F	frequencyV = M::vectorizeOne( this->GetFrequency() );
					typename M::Vector4F	lacunarityV = M::vectorizeOne( this->GetLacunarity() );
					uint32					octaveCount = this->GetOctaveCount();
					typename M::Vector4F	persistenceV = M::vectorizeOne( this->GetPersistence() );
					typename M::Vector4I	seedV = M::vectorizeOne( this->GetSeed() );
					NoiseQuality			noiseQuality = this->GetNoiseQuality();

					typename M::Vector4F	valueV = M::constZeroF();
					typename M::Vector4F	signalV = M::constZeroF();
					typename M::Vector4F	curPersistenceV = M::constOneF();
					typename M::Vector4F	nxV;
					typename M::Vector4F	nyV;

					for( uint32 curOctave = 0; curOctave < octaveCount; curOctave++ ) 
					{
						// Make sure that these floating-point values have the same range as a 32-
						// bit integer so that we can pass them to the coherent-noise functions.
						//nx = M::MakeInt32Range( x );
						nxV = xV;
						nyV = yV;
						
						// Get the coherent-noise value from the input value and add it to the
						// final result.
						signalV = Noise::GradientCoherentNoise4( nxV, nyV, seedV, noiseQuality );
						signalV = M::subtract( M::multiply( M::constTwoF(), M::abs( signalV ) ), M::constOneF() );
						valueV = M::add( valueV, M::multiply( signalV, curPersistenceV ) );

						// Prepare the next octave.
						xV = M::multiply( xV, lacunarityV );
						yV = M::multiply( yV, lacunarityV );
						curPersistenceV = M::multiply( curPersistenceV, persistenceV );
						seedV = M::add( seedV, M::constOneI() );
					}

					valueV = M::add( valueV, M::vectorizeOne( ValueType( 0.5 ) ) );
					M::storeToMemory( output, valueV );
				}

			};

			
			
			template< typename ValueT >
			class BillowImpl< ValueT, 3, 4 >: public Module< ValueT, 3 >, public BillowBase< ValueT >
			{
				public:

				typedef ValueT										ValueType;
				static const unsigned								Dimension = 3;
				typedef Module< ValueType, Dimension >				ModuleType;
				typedef BillowBase< ValueType >						BillowBaseType;
				typedef BillowImpl< ValueType, Dimension, 4 >		ThisType;
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

					typename M::Vector4F		coordV = M::vectorize( x, y, z );
					typename M::Vector4F		frequencyV = M::vectorizeOne( frequency );
					typename M::Vector4F		lacunarityV = M::vectorizeOne( lacunarity );

					coordV = M::multiply( coordV, frequencyV );
					
					for( uint32 curOctave = 0; curOctave < octaveCount; curOctave++ )
					{
						// Make sure that these floating-pouint32 values have the same range as a 32-
						// bit uint32eger so that we can pass them to the coherent-noise functions.
						/*nx = M::MakeInt32Range( x );
						ny = M::MakeInt32Range( y );
						nz = M::MakeInt32Range( z );*/
						
						// Get the coherent-noise value from the input value and add it to the
						// final result.
						octaveSeed = (seed + curOctave) & 0xffffffff;
						signal = Noise::GradientCoherentNoise( coordV, octaveSeed, noiseQuality );
						signal = ValueType( 2.0 ) * fabs( signal ) - ValueType( 1.0 );
						value += signal * curPersistence;

						// Prepare the next octave.
						coordV = M::multiply( coordV, lacunarityV );
						curPersistence *= persistence;
					}
					value += ValueType( 0.5 );

					return value;
				}

				void
				GetValue4( const ValueType* inputX, const ValueType* inputY, const ValueType* inputZ, ValueType* output )
				{
					typename M::Vector4F	xV = M::loadFromMemory( inputX );
					typename M::Vector4F	yV = M::loadFromMemory( inputY );
					typename M::Vector4F	zV = M::loadFromMemory( inputZ );

					typename M::Vector4F	frequencyV = M::vectorizeOne( this->GetFrequency() );
					typename M::Vector4F	lacunarityV = M::vectorizeOne( this->GetLacunarity() );
					uint32					octaveCount = this->GetOctaveCount();
					typename M::Vector4F	persistenceV = M::vectorizeOne( this->GetPersistence() );
					typename M::Vector4I	seedV = M::vectorizeOne( this->GetSeed() );
					NoiseQuality			noiseQuality = this->GetNoiseQuality();

					typename M::Vector4F	valueV = M::constZeroF();
					typename M::Vector4F	signalV = M::constZeroF();
					typename M::Vector4F	curPersistenceV = M::constOneF();
					typename M::Vector4F	nxV;
					typename M::Vector4F	nyV;
					typename M::Vector4F	nzV;

					for( uint32 curOctave = 0; curOctave < octaveCount; curOctave++ ) 
					{
						// Make sure that these floating-point values have the same range as a 32-
						// bit integer so that we can pass them to the coherent-noise functions.
						//nx = M::MakeInt32Range( x );
						nxV = xV;
						nyV = yV;
						nzV = zV;
						
						// Get the coherent-noise value from the input value and add it to the
						// final result.
						signalV = Noise::GradientCoherentNoise4( nxV, nyV, nzV, seedV, noiseQuality );
						signalV = M::subtract( M::multiply( M::constTwoF(), M::abs( signalV ) ), M::constOneF() );
						valueV = M::add( valueV, M::multiply( signalV, curPersistenceV ) );

						// Prepare the next octave.
						xV = M::multiply( xV, lacunarityV );
						yV = M::multiply( yV, lacunarityV );
						zV = M::multiply( zV, lacunarityV );
						curPersistenceV = M::multiply( curPersistenceV, persistenceV );
						seedV = M::add( seedV, M::constOneI() );
					}

					valueV = M::add( valueV, M::vectorizeOne( ValueType( 0.5 ) ) );
					M::storeToMemory( output, valueV );
				}

			};
			
			
			
			template< typename ValueT >
			class BillowImpl< ValueT, 4, 4 >: public Module< ValueT, 4 >, public BillowBase< ValueT >
			{
				public:

				typedef ValueT										ValueType;
				static const unsigned								Dimension = 4;
				typedef Module< ValueType, Dimension >				ModuleType;
				typedef BillowBase< ValueType >						BillowBaseType;
				typedef BillowImpl< ValueType, Dimension, 4 >		ThisType;
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

					typename M::Vector4F		coordV = M::vectorize( x, y, z, w );
					typename M::Vector4F		frequencyV = M::vectorizeOne( frequency );
					typename M::Vector4F		lacunarityV = M::vectorizeOne( lacunarity );

					coordV = M::multiply( coordV, frequencyV );

					for( uint32 curOctave = 0; curOctave < octaveCount; curOctave++ )
					{
						// Make sure that these floating-pouint32 values have the same range as a 32-
						// bit uint32eger so that we can pass them to the coherent-noise functions.
						/*nx = M::MakeInt32Range( x );
						ny = M::MakeInt32Range( y );
						nz = M::MakeInt32Range( z );
						nw = M::MakeInt32Range( w );*/

						// Get the coherent-noise value from the input value and add it to the
						// final result.
						octaveSeed = (seed + curOctave) & 0xffffffff;
						signal = Noise::GradientCoherentNoise( coordV, octaveSeed, noiseQuality );
						signal = ValueType( 2.0 ) * fabs( signal ) - ValueType( 1.0 );
						value += signal * curPersistence;

						// Prepare the next octave.
						coordV = M::multiply( coordV, lacunarityV );
						curPersistence *= persistence;
					}
					value += ValueType( 0.5 );

					return value;
				}

				void
				GetValue4( const ValueType* inputX, const ValueType* inputY, const ValueType* inputZ, const ValueType* inputW,
						   ValueType* output )
				{
					typename M::Vector4F	xV = M::loadFromMemory( inputX );
					typename M::Vector4F	yV = M::loadFromMemory( inputY );
					typename M::Vector4F	zV = M::loadFromMemory( inputZ );
					typename M::Vector4F	wV = M::loadFromMemory( inputW );

					typename M::Vector4F	frequencyV = M::vectorizeOne( this->GetFrequency() );
					typename M::Vector4F	lacunarityV = M::vectorizeOne( this->GetLacunarity() );
					uint32					octaveCount = this->GetOctaveCount();
					typename M::Vector4F	persistenceV = M::vectorizeOne( this->GetPersistence() );
					typename M::Vector4I	seedV = M::vectorizeOne( this->GetSeed() );
					NoiseQuality			noiseQuality = this->GetNoiseQuality();

					typename M::Vector4F	valueV = M::constZeroF();
					typename M::Vector4F	signalV = M::constZeroF();
					typename M::Vector4F	curPersistenceV = M::constOneF();
					typename M::Vector4F	nxV;
					typename M::Vector4F	nyV;
					typename M::Vector4F	nzV;
					typename M::Vector4F	nwV;

					for( uint32 curOctave = 0; curOctave < octaveCount; curOctave++ ) 
					{
						// Make sure that these floating-point values have the same range as a 32-
						// bit integer so that we can pass them to the coherent-noise functions.
						//nx = M::MakeInt32Range( x );
						nxV = xV;
						nyV = yV;
						nzV = zV;
						nwV = wV;
						
						// Get the coherent-noise value from the input value and add it to the
						// final result.
						signalV = Noise::GradientCoherentNoise4( nxV, nyV, nzV, nwV, seedV, noiseQuality );
						signalV = M::subtract( M::multiply( M::constTwoF(), M::abs( signalV ) ), M::constOneF() );
						valueV = M::add( valueV, M::multiply( signalV, curPersistenceV ) );

						// Prepare the next octave.
						xV = M::multiply( xV, lacunarityV );
						yV = M::multiply( yV, lacunarityV );
						zV = M::multiply( zV, lacunarityV );
						wV = M::multiply( wV, lacunarityV );
						curPersistenceV = M::multiply( curPersistenceV, persistenceV );
						seedV = M::add( seedV, M::constOneI() );
					}

					valueV = M::add( valueV, M::vectorizeOne( ValueType( 0.5 ) ) );
					M::storeToMemory( output, valueV );
				}

			};

		}

	}

}
