#pragma once


// libnoise
#include <noise/debug/Debug.hpp>
#include <noise/math/Math.hpp>
#include <noise/math/Interpolations.hpp>
#include <noise/module/perlin/VectorTable.hpp>




namespace noise
{

	namespace module
	{

		namespace perlin
		{

			template< typename ValueT >
			class NoiseGen< ValueT, 1 >
			{

				public:
				
				typedef ValueT									ValueType;
				static const unsigned int						Dimension = 1;
				typedef math::Math< ValueType >					M;
				typedef math::Interpolations< ValueType >		Interp;
				typedef VectorTable< ValueType >				Table;
				
				
				
				public:
				
				static inline
				ValueType
				GradientCoherentNoise( ValueType x, int seed = 0, NoiseQuality noiseQuality = QUALITY_STD )
				{
					// Create a unit-length cube aligned along an integer boundary.  This cube
					// surrounds the input point.
					int	x0 = ( x > 0.0 ? (int) x: (int) x - 1 );
					int	x1 = x0 + 1;
					
					// Map the difference between the coordinates of the input value and the
					// coordinates of the cube's outer-lower-left vertex onto an S-curve.
					ValueType xs = 0.0f;
					
					switch( noiseQuality )
					{
						case QUALITY_FAST:

						xs = ( x - (ValueType) x0 );
						
						break;


						case QUALITY_STD:

						xs = Interp::SCurve3( x - (ValueType) x0 );
						
						break;


						case QUALITY_BEST:

						xs = Interp::SCurve5( x - (ValueType) x0 );
						
						break;
					}

					// Now calculate the noise values at each vertex of the cube.  To generate
					// the coherent-noise value at the input point, interpolate these eight
					// noise values using the S-curve value as the interpolant (trilinear
					// interpolation.)
					ValueType n0, n1;

					n0   = GradientNoise( x, x0, seed );
					n1   = GradientNoise( x, x1, seed );
					
					return Interp::LinearInterp( n0, n1, xs );
				}
				
				static inline
				ValueType
				GradientNoise( ValueType fx, int ix, int seed )
				{
					// Randomly generate a gradient vector given the integer coordinates of the
					// input value.  This implementation generates a random number and uses it
					// as an index into a normalized-vector lookup table.
					int vectorIndex = (
					  X_NOISE_GEN    * ix
					+ SEED_NOISE_GEN * seed)
					& 0xffffffff;
					
					vectorIndex ^= (vectorIndex >> SHIFT_NOISE_GEN);
					vectorIndex &= 0xff;

					ValueType	xvGradient = Table::values()[ (vectorIndex << 2) ];
					
					// Set up us another vector equal to the distance between the two vectors
					// passed to this function.
					ValueType	xvPoint = (fx - (ValueType) ix);
					
					// Now compute the dot product of the gradient vector with the distance
					// vector.  The resulting value is gradient noise.  Apply a scaling value
					// so that this noise value ranges from -1.0 to 1.0.
					return xvGradient * xvPoint * 2.12f;
				}

			};



			template< typename ValueT >
			class NoiseGen< ValueT, 2 >
			{

				public:
				
				typedef ValueT									ValueType;
				static const unsigned int						Dimension = 2;
				typedef math::Math< ValueType >					M;
				typedef math::Interpolations< ValueType >		Interp;
				typedef VectorTable< ValueType >				Table;



				public:
				
				static inline
				ValueType
				GradientCoherentNoise( typename M::Vector4F& coordV, int seed = 0, NoiseQuality noiseQuality = QUALITY_STD )
				{
					// Create a unit-length cube aligned along an integer boundary.  This cube
					// surrounds the input point.
					typename M::Vector4I	coord0V = M::floatToIntTruncated( coordV );
					coord0V = M::subtract( coord0V, M::signToOne( coordV ) );
					typename M::Vector4I	coord1V = M::add( coord0V, M::constOneI() );

					// Map the difference between the coordinates of the input value and the
					// coordinates of the cube's outer-lower-left vertex onto an S-curve.
					typename M::Vector4F		xy__0 = M::subtract( coordV, M::intToFloat( coord0V ) );
					typename M::Vector4F		xy__1 = M::subtract( coordV, M::intToFloat( coord1V ) );
					typename M::Vector4F		sCoordV = xy__0;

					// Interpolate for noise quality
					switch( noiseQuality )
					{
						case QUALITY_FAST:

						// do nothing more

						break;


						case QUALITY_STD:

						sCoordV = Interp::SCurve3V( sCoordV );

						break;


						case QUALITY_BEST:

						sCoordV = Interp::SCurve5V( sCoordV );

						break;
					}

					// Calculate partial gradient table indexes
					static VECTOR4_ALIGN( uint32		noiseGenA[ 4 ] ) = { X_NOISE_GEN, Y_NOISE_GEN, 0, 0 };
					typename M::Vector4I	noiseGenV = M::loadFromMemory( noiseGenA );
					typename M::Vector4I	seedV = M::vectorizeOne( SEED_NOISE_GEN * seed );

					coord0V = M::multiply( coord0V, noiseGenV );
					coord1V = M::multiply( coord1V, noiseGenV );
					typename M::Vector4I	ix0x1y0y1 = M::interleaveLo( coord0V, coord1V );
					
					// Now calculate the noise values at each vertex of the cube.  To generate
					// the coherent-noise value at the input point, interpolate these eight
					// noise values using the S-curve value as the interpolant (trilinear
					// interpolation.)
					
					typename M::Vector4F		x0y0x1y1 = M::template shuffle< 0, 1, 0, 1 >( xy__0, xy__1 );
					
					typename M::Vector4I		indexBaseV;
					typename M::Vector4F		interpV;
					typename M::Vector4F		n;
					
					indexBaseV = seedV;
					calculateNoisePart< Shuffle_0_3 >( x0y0x1y1, indexBaseV, ix0x1y0y1, n );

					typename M::Vector4F		n1 = M::template shuffle< 0, 0, 1, 1 >( n, n );
					typename M::Vector4F		n2 = M::template shuffle< 2, 2, 3, 3 >( n, n );

					interpV = M::distribute1st( sCoordV );
					typename M::Vector4F		nx = Interp::LinearInterpV( n1, n2, interpV );
					typename M::Vector4F		nx1 = M::template shuffle< 0, 0, 0, 0 >( nx, nx );
					typename M::Vector4F		nx2 = M::template shuffle< 2, 2, 2, 2 >( nx, nx );

					interpV = M::distribute2nd( sCoordV );
					typename M::Vector4F		ny = Interp::LinearInterpV( nx1, nx2, interpV );

					return M::extract1st( ny );
				}



			private:

				struct Shuffle_0_3
				{
					typedef typename M::template Order< 0, 1, 0, 0 >		Shuffle_00;
					typedef typename M::template Order< 0, 3, 0, 0 >		Shuffle_01;
					typedef typename M::template Order< 2, 1, 0, 0 >		Shuffle_10;
					typedef typename M::template Order< 2, 3, 0, 0 >		Shuffle_11;
				};

				

				template< typename ShufflePolicy >
				static inline
				void
				calculateNoisePart( typename M::Vector4F& x0y0x1y1,
									typename M::Vector4I& indexBaseV, typename M::Vector4I& ix0x1y0y1,
									typename M::Vector4F& n0 )
				{
					// calculate table indices
					ValueType*		tableV = Table::values();

					typename M::Vector4I		tableIndexV = indexBaseV;
					tableIndexV = M::add( tableIndexV, M::template shuffle< 0, 0, 1, 1 >( ix0x1y0y1 ) );
					tableIndexV = M::add( tableIndexV, M::template shuffle< 2, 3, 2, 3 >( ix0x1y0y1 ) );
					tableIndexV = M::bitXor( tableIndexV, M::shiftRightLogical( tableIndexV, SHIFT_NOISE_GEN ) );
					tableIndexV = M::bitAnd( tableIndexV, M::vectorizeOne( 0xff ) );
					tableIndexV = M::multiply( tableIndexV, M::constFourI() );

					VECTOR4_ALIGN( uint32	tableIndexA[ 4 ] );
					M::storeToMemory( tableIndexA, tableIndexV );
					
					// multiply with gradients
					typename M::Vector4F		gradient;

					typename M::Vector4F		n1 = M::template shuffle< typename ShufflePolicy::Shuffle_00 >( x0y0x1y1, M::constZeroF() );
					gradient = M::loadFromMemory( tableV + tableIndexA[ 0 ] );
					n1 = M::multiply( n1, gradient );

					typename M::Vector4F		n2 = M::template shuffle< typename ShufflePolicy::Shuffle_01 >( x0y0x1y1, M::constZeroF() );
					gradient = M::loadFromMemory( tableV + tableIndexA[ 1 ] );
					n2 = M::multiply( n2, gradient );

					typename M::Vector4F		nd1 = M::reduce( n1, n2 );
					
					n1 = M::template shuffle< typename ShufflePolicy::Shuffle_10 >( x0y0x1y1, M::constZeroF() );
					gradient = M::loadFromMemory( tableV + tableIndexA[ 2 ] );
					n1 = M::multiply( n1, gradient );

					n2 = M::template shuffle< typename ShufflePolicy::Shuffle_11 >( x0y0x1y1, M::constZeroF() );
					gradient = M::loadFromMemory( tableV + tableIndexA[ 3 ] );
					n2 = M::multiply( n2, gradient );

					typename M::Vector4F	nd2 = M::reduce( n1, n2 );
					typename M::Vector4F	nd = M::reduce( nd1, nd2 );
					
					// scale noise values
					typename M::Vector4F		noiseScale = M::vectorizeOne( ValueType( 2.12 ) );
					n0 = M::multiply( nd, noiseScale );
				}

			};



			template< typename ValueT >
			class NoiseGen< ValueT, 3 >
			{

				public:
				
				typedef ValueT									ValueType;
				static const unsigned int						Dimension = 3;
				typedef math::Math< ValueType >					M;
				typedef math::Interpolations< ValueType >		Interp;
				typedef VectorTable< ValueType >				Table;



				public:
				
				static inline
				ValueType
				GradientCoherentNoise( typename M::Vector4F& coordV, int seed = 0, NoiseQuality noiseQuality = QUALITY_STD )
				{
					// Create a unit-length cube aligned along an integer boundary.  This cube
					// surrounds the input point.
					typename M::Vector4I	coord0V = M::floatToIntTruncated( coordV );
					coord0V = M::subtract( coord0V, M::signToOne( coordV ) );
					typename M::Vector4I	coord1V = M::add( coord0V, M::constOneI() );

					// Map the difference between the coordinates of the input value and the
					// coordinates of the cube's outer-lower-left vertex onto an S-curve.
					typename M::Vector4F		xyz_0 = M::subtract( coordV, M::intToFloat( coord0V ) );
					typename M::Vector4F		xyz_1 = M::subtract( coordV, M::intToFloat( coord1V ) );
					typename M::Vector4F		sCoordV = xyz_0;

					// Interpolate for noise quality
					switch( noiseQuality )
					{
						case QUALITY_FAST:

						// do nothing more

						break;


						case QUALITY_STD:

						sCoordV = Interp::SCurve3V( sCoordV );

						break;


						case QUALITY_BEST:

						sCoordV = Interp::SCurve5V( sCoordV );

						break;
					}

					// Calculate partial gradient table indexes
					static VECTOR4_ALIGN( uint32		noiseGenA[ 4 ] ) = { X_NOISE_GEN, Y_NOISE_GEN, Z_NOISE_GEN, 0 };
					typename M::Vector4I	noiseGenV = M::loadFromMemory( noiseGenA );
					typename M::Vector4I	seedV = M::vectorizeOne( SEED_NOISE_GEN * seed );

					coord0V = M::multiply( coord0V, noiseGenV );
					coord1V = M::multiply( coord1V, noiseGenV );
					typename M::Vector4I	iy0y1z0z1 = M::interleaveHi( M::template shuffle< 0, 3, 1, 2 >( coord0V ), M::template shuffle< 0, 3, 1, 2 >( coord1V ) );
					
					// Now calculate the noise values at each vertex of the cube.  To generate
					// the coherent-noise value at the input point, interpolate these eight
					// noise values using the S-curve value as the interpolant (trilinear
					// interpolation.)
					
					typename M::Vector4F		x0y0x1y1 = M::template shuffle< 0, 1, 0, 1 >( xyz_0, xyz_1 );
					typename M::Vector4F		z0__z1__ = M::template shuffle< 2, 3, 2, 3 >( xyz_0, xyz_1 );
					
					typename M::Vector4I		indexBaseV;
					typename M::Vector4F		interpV;
					typename M::Vector4F		n1;
					typename M::Vector4F		n2;

					indexBaseV = M::add( seedV, M::distribute1st( coord0V ) );
					calculateNoisePart< Shuffle_0_3 >( x0y0x1y1, z0__z1__, indexBaseV, iy0y1z0z1, n1 );

					indexBaseV = M::add( seedV, M::distribute1st( coord1V ) );
					calculateNoisePart< Shuffle_4_7 >( x0y0x1y1, z0__z1__, indexBaseV, iy0y1z0z1, n2 );

					interpV = M::distribute1st( sCoordV );
					typename M::Vector4F		nx = Interp::LinearInterpV( n1, n2, interpV );
					typename M::Vector4F		nx1 = M::template shuffle< 0, 0, 1, 1 >( nx, nx );
					typename M::Vector4F		nx2 = M::template shuffle< 2, 2, 3, 3 >( nx, nx );

					interpV = M::distribute2nd( sCoordV );
					typename M::Vector4F		ny = Interp::LinearInterpV( nx1, nx2, interpV );
					typename M::Vector4F		ny1 = M::template shuffle< 0, 0, 0, 0 >( ny, ny );
					typename M::Vector4F		ny2 = M::template shuffle< 2, 2, 2, 2 >( ny, ny );

					interpV = M::distribute3rd( sCoordV );
					typename M::Vector4F		nz = Interp::LinearInterpV( ny1, ny2, interpV );

					return M::extract1st( nz );
				}



			private:

				struct Shuffle_0_3
				{
					typedef typename M::template Order< 0, 1, 0, 0 >		Shuffle_00;
					typedef typename M::template Order< 0, 1, 2, 0 >		Shuffle_01;
					typedef typename M::template Order< 0, 3, 0, 0 >		Shuffle_10;
					typedef typename M::template Order< 0, 3, 2, 0 >		Shuffle_11;
				};

				struct Shuffle_4_7
				{
					typedef typename M::template Order< 2, 1, 0, 0 >		Shuffle_00;
					typedef typename M::template Order< 2, 1, 2, 0 >		Shuffle_01;
					typedef typename M::template Order< 2, 3, 0, 0 >		Shuffle_10;
					typedef typename M::template Order< 2, 3, 2, 0 >		Shuffle_11;
				};



				template< typename ShufflePolicy >
				static inline
				void
				calculateNoisePart( typename M::Vector4F& x0y0x1y1, typename M::Vector4F& z0__z1__,
									typename M::Vector4I& indexBaseV, typename M::Vector4I& iy0y1z0z1,
									typename M::Vector4F& n0 )
				{
					// calculate table indices
					ValueType*		tableV = Table::values();

					typename M::Vector4I		tableIndexV = indexBaseV;
					tableIndexV = M::add( tableIndexV, M::template shuffle< 0, 0, 1, 1 >( iy0y1z0z1 ) );
					tableIndexV = M::add( tableIndexV, M::template shuffle< 2, 3, 2, 3 >( iy0y1z0z1 ) );
					tableIndexV = M::bitXor( tableIndexV, M::shiftRightLogical( tableIndexV, SHIFT_NOISE_GEN ) );
					tableIndexV = M::bitAnd( tableIndexV, M::vectorizeOne( 0xff ) );
					tableIndexV = M::multiply( tableIndexV, M::constFourI() );

					VECTOR4_ALIGN( uint32	tableIndexA[ 4 ] );
					M::storeToMemory( tableIndexA, tableIndexV );
					
					// multiply with gradients
					typename M::Vector4F		gradient;

					typename M::Vector4F		n1 = M::template shuffle< typename ShufflePolicy::Shuffle_00 >( x0y0x1y1, z0__z1__ );
					gradient = M::loadFromMemory( tableV + tableIndexA[ 0 ] );
					n1 = M::multiply( n1, gradient );

					typename M::Vector4F		n2 = M::template shuffle< typename ShufflePolicy::Shuffle_01 >( x0y0x1y1, z0__z1__ );
					gradient = M::loadFromMemory( tableV + tableIndexA[ 1 ] );
					n2 = M::multiply( n2, gradient );

					typename M::Vector4F		nd1 = M::reduce( n1, n2 );
					
					n1 = M::template shuffle< typename ShufflePolicy::Shuffle_10 >( x0y0x1y1, z0__z1__ );
					gradient = M::loadFromMemory( tableV + tableIndexA[ 2 ] );
					n1 = M::multiply( n1, gradient );

					n2 = M::template shuffle< typename ShufflePolicy::Shuffle_11 >( x0y0x1y1, z0__z1__ );
					gradient = M::loadFromMemory( tableV + tableIndexA[ 3 ] );
					n2 = M::multiply( n2, gradient );

					typename M::Vector4F	nd2 = M::reduce( n1, n2 );
					typename M::Vector4F	nd = M::reduce( nd1, nd2 );
					
					// scale noise values
					typename M::Vector4F		noiseScale = M::vectorizeOne( ValueType( 2.12 ) );
					n0 = M::multiply( nd, noiseScale );
				}

			};



			template< typename ValueT >
			class NoiseGen< ValueT, 4 >
			{

				public:
				
				typedef ValueT									ValueType;
				static const unsigned int						Dimension = 4;
				typedef math::Math< ValueType >					M;
				typedef math::Interpolations< ValueType >		Interp;
				typedef VectorTable< ValueType >				Table;



				public:
				
				static inline
				ValueType
				GradientCoherentNoise( typename M::Vector4F& coordV, int seed = 0, NoiseQuality noiseQuality = QUALITY_STD )
				{
					// Create a unit-length cube aligned along an integer boundary.  This cube
					// surrounds the input point.
					typename M::Vector4I	coord0V = M::floatToIntTruncated( coordV );
					coord0V = M::subtract( coord0V, M::signToOne( coordV ) );
					typename M::Vector4I	coord1V = M::add( coord0V, M::constOneI() );

					// Map the difference between the coordinates of the input value and the
					// coordinates of the cube's outer-lower-left vertex onto an S-curve.
					typename M::Vector4F		xyzw0 = M::subtract( coordV, M::intToFloat( coord0V ) );
					typename M::Vector4F		xyzw1 = M::subtract( coordV, M::intToFloat( coord1V ) );
					typename M::Vector4F		sCoordV = xyzw0;
					
					// Interpolate for noise quality
					switch( noiseQuality )
					{
						case QUALITY_FAST:

						// do nothing more

						break;


						case QUALITY_STD:

						sCoordV = Interp::SCurve3V( sCoordV );

						break;


						case QUALITY_BEST:

						sCoordV = Interp::SCurve5V( sCoordV );

						break;
					}

					// Calculate partial gradient table indexes
					static VECTOR4_ALIGN( uint32		noiseGenA[ 4 ] ) = { X_NOISE_GEN, Y_NOISE_GEN, Z_NOISE_GEN, W_NOISE_GEN };
					typename M::Vector4I	noiseGenV = M::loadFromMemory( noiseGenA );
					typename M::Vector4I	seedV = M::vectorizeOne( SEED_NOISE_GEN * seed );

					coord0V = M::multiply( coord0V, noiseGenV );
					coord1V = M::multiply( coord1V, noiseGenV );
					typename M::Vector4I	iz0z1w0w1 = M::interleaveHi( coord0V, coord1V );

					// Now calculate the noise values at each vertex of the cube.  To generate
					// the coherent-noise value at the input point, interpolate these eight
					// noise values using the S-curve value as the interpolant (trilinear
					// interpolation.)
					typename M::Vector4F		x0y0x1y1 = M::template shuffle< 0, 1, 0, 1 >( xyzw0, xyzw1 );
					typename M::Vector4F		z0w0z1w1 = M::template shuffle< 2, 3, 2, 3 >( xyzw0, xyzw1 );

					typename M::Vector4I		indexBaseV;
					typename M::Vector4F		interpV;
					typename M::Vector4F		n1;
					typename M::Vector4F		n2;

					indexBaseV = M::add( seedV, M::add( M::distribute1st( coord0V ), M::distribute2nd( coord0V ) ) );
					calculateNoisePart< Shuffle_0_3 >( x0y0x1y1, z0w0z1w1, indexBaseV, iz0z1w0w1, n1 );

					indexBaseV = M::add( seedV, M::add( M::distribute1st( coord1V ), M::distribute2nd( coord0V ) ) );
					calculateNoisePart< Shuffle_8_11 >( x0y0x1y1, z0w0z1w1, indexBaseV, iz0z1w0w1, n2 );

					interpV = M::distribute1st( sCoordV );
					typename M::Vector4F		nx1 = Interp::LinearInterpV( n1, n2, interpV );

					indexBaseV = M::add( seedV, M::add( M::distribute1st( coord0V ), M::distribute2nd( coord1V ) ) );
					calculateNoisePart< Shuffle_4_7 >( x0y0x1y1, z0w0z1w1, indexBaseV, iz0z1w0w1, n1 );

					indexBaseV = M::add( seedV, M::add( M::distribute1st( coord1V ), M::distribute2nd( coord1V ) ) );
					calculateNoisePart< Shuffle_12_15 >( x0y0x1y1, z0w0z1w1, indexBaseV, iz0z1w0w1, n2 );

					typename M::Vector4F		nx2 = Interp::LinearInterpV( n1, n2, interpV );

					interpV = M::distribute2nd( sCoordV );
					typename M::Vector4F		ny = Interp::LinearInterpV( nx1, nx2, interpV );
					typename M::Vector4F		ny1 = M::template shuffle< 0, 0, 1, 1 >( ny, ny );
					typename M::Vector4F		ny2 = M::template shuffle< 2, 2, 3, 3 >( ny, ny );

					interpV = M::distribute3rd( sCoordV );
					typename M::Vector4F		nz = Interp::LinearInterpV( ny1, ny2, interpV );
					typename M::Vector4F		nz1 = M::template shuffle< 0, 0, 0, 0 >( nz, nz );
					typename M::Vector4F		nz2 = M::template shuffle< 2, 2, 2, 2 >( nz, nz );

					interpV = M::distribute4th( sCoordV );
					typename M::Vector4F		nw = Interp::LinearInterpV( nz1, nz2, interpV );

					return M::extract1st( nw );
				}



			private:

				struct Shuffle_0_3
				{
					typedef typename M::template Order< 0, 1, 0, 1 >		Shuffle_00;
					typedef typename M::template Order< 0, 1, 0, 3 >		Shuffle_01;
					typedef typename M::template Order< 0, 1, 2, 1 >		Shuffle_10;
					typedef typename M::template Order< 0, 1, 2, 3 >		Shuffle_11;
				};

				struct Shuffle_4_7
				{
					typedef typename M::template Order< 0, 3, 0, 1 >		Shuffle_00;
					typedef typename M::template Order< 0, 3, 0, 3 >		Shuffle_01;
					typedef typename M::template Order< 0, 3, 2, 1 >		Shuffle_10;
					typedef typename M::template Order< 0, 3, 2, 3 >		Shuffle_11;
				};

				struct Shuffle_8_11
				{
					typedef typename M::template Order< 2, 1, 0, 1 >		Shuffle_00;
					typedef typename M::template Order< 2, 1, 0, 3 >		Shuffle_01;
					typedef typename M::template Order< 2, 1, 2, 1 >		Shuffle_10;
					typedef typename M::template Order< 2, 1, 2, 3 >		Shuffle_11;
				};

				struct Shuffle_12_15
				{
					typedef typename M::template Order< 2, 3, 0, 1 >		Shuffle_00;
					typedef typename M::template Order< 2, 3, 0, 3 >		Shuffle_01;
					typedef typename M::template Order< 2, 3, 2, 1 >		Shuffle_10;
					typedef typename M::template Order< 2, 3, 2, 3 >		Shuffle_11;
				};



				template< typename ShufflePolicy >
				static inline
				void
				calculateNoisePart( typename M::Vector4F& x0y0x1y1, typename M::Vector4F& z0w0z1w1,
									typename M::Vector4I& indexBaseV, typename M::Vector4I& iz0z1w0w1,
									typename M::Vector4F& n0 )
				{
					// calculate table indices
					ValueType*		tableV = Table::values();

					typename M::Vector4I		tableIndexV = indexBaseV;
					tableIndexV = M::add( tableIndexV, M::template shuffle< 0, 0, 1, 1 >( iz0z1w0w1 ) );
					tableIndexV = M::add( tableIndexV, M::template shuffle< 2, 3, 2, 3 >( iz0z1w0w1 ) );
					tableIndexV = M::bitXor( tableIndexV, M::shiftRightLogical( tableIndexV, SHIFT_NOISE_GEN ) );
					tableIndexV = M::bitAnd( tableIndexV, M::vectorizeOne( 0xff ) );
					tableIndexV = M::multiply( tableIndexV, M::constFourI() );

					VECTOR4_ALIGN( uint32	tableIndexA[ 4 ] );
					M::storeToMemory( tableIndexA, tableIndexV );
					
					// multiply with gradients
					typename M::Vector4F		gradient;

					typename M::Vector4F		n1 = M::template shuffle< typename ShufflePolicy::Shuffle_00 >( x0y0x1y1, z0w0z1w1 );
					gradient = M::loadFromMemory( tableV + tableIndexA[ 0 ] );
					n1 = M::multiply( n1, gradient );

					typename M::Vector4F		n2 = M::template shuffle< typename ShufflePolicy::Shuffle_01 >( x0y0x1y1, z0w0z1w1 );
					gradient = M::loadFromMemory( tableV + tableIndexA[ 1 ] );
					n2 = M::multiply( n2, gradient );

					typename M::Vector4F		nd1 = M::reduce( n1, n2 );
					
					n1 = M::template shuffle< typename ShufflePolicy::Shuffle_10 >( x0y0x1y1, z0w0z1w1 );
					gradient = M::loadFromMemory( tableV + tableIndexA[ 2 ] );
					n1 = M::multiply( n1, gradient );

					n2 = M::template shuffle< typename ShufflePolicy::Shuffle_11 >( x0y0x1y1, z0w0z1w1 );
					gradient = M::loadFromMemory( tableV + tableIndexA[ 3 ] );
					n2 = M::multiply( n2, gradient );

					typename M::Vector4F	nd2 = M::reduce( n1, n2 );
					typename M::Vector4F	nd = M::reduce( nd1, nd2 );
					
					// scale noise values
					typename M::Vector4F		noiseScale = M::vectorizeOne( ValueType( 2.12 ) );
					n0 = M::multiply( nd, noiseScale );
				}

			};

		}

	}

}