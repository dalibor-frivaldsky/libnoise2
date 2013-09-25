#pragma once


// libnoise
#include <noise2/debug/Debug.hpp>
#include <noise2/math/Math.hpp>
#include <noise2/math/Interpolations.hpp>
#include <noise2/module/perlin/VectorTable.hpp>




namespace noise2
{

	namespace module
	{

		namespace perlin
		{

			template< typename ValueT >
			class NoiseGenImpl< ValueT, 1, 4 >
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
				GradientCoherentNoise( ValueType x, uint32 seed = 0, NoiseQuality noiseQuality = QUALITY_STD )
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



			private:
				
				static inline
				ValueType
				GradientNoise( ValueType fx, int32 ix, uint32 seed )
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



			public:

				static inline
				typename M::Vector4F
				GradientCoherentNoise4( const typename M::Vector4F& xV, const typename M::Vector4I& seedV,
										NoiseQuality noiseQuality = QUALITY_STD )
				{
					// Create a unit-length cube aligned along an integer boundary.  This cube
					// surrounds the input point.
					typename M::Vector4I	x0V = M::floatToIntTruncated( xV );
					x0V = M::subtract( x0V, M::signToOne( xV ) );
					typename M::Vector4I	x1V = M::add( x0V, M::constOneI() );

					// Map the difference between the coordinates of the input value and the
					// coordinates of the cube's outer-lower-left vertex onto an S-curve.
					typename M::Vector4F	xsV = M::subtract( xV, M::intToFloat( x0V ) );
					
					switch( noiseQuality )
					{
						case QUALITY_FAST:

						// do nothing more
						
						break;


						case QUALITY_STD:

						xsV = Interp::SCurve3V( xsV );
						
						break;


						case QUALITY_BEST:

						xsV = Interp::SCurve5V( xsV );
						
						break;
					}

					// Now calculate the noise values at each vertex of the cube.  To generate
					// the coherent-noise value at the input point, interpolate these eight
					// noise values using the S-curve value as the interpolant (trilinear
					// interpolation.)
					typename M::Vector4F	n0V;
					typename M::Vector4F	n1V;
					
					n0V = GradientNoise4( xV, x0V, seedV );
					n1V = GradientNoise4( xV, x1V, seedV );

					return Interp::LinearInterpV( n0V, n1V, xsV );
				}



			private:
				
				static inline
				typename M::Vector4F
				GradientNoise4( const typename M::Vector4F& fxV, const typename M::Vector4I& ixV,
								const typename M::Vector4I& seedV )
				{
					static VECTOR4_ALIGN( uint32	seedNoiseGenA[ 4 ] ) = { SEED_NOISE_GEN, SEED_NOISE_GEN, SEED_NOISE_GEN, SEED_NOISE_GEN };
					static VECTOR4_ALIGN( uint32	xNoiseGenA[ 4 ] ) = { X_NOISE_GEN, X_NOISE_GEN, X_NOISE_GEN, X_NOISE_GEN };
					typename M::Vector4I	seedNoiseGenV = M::loadFromMemory( seedNoiseGenA );
					typename M::Vector4I	xNoiseGenV = M::loadFromMemory( xNoiseGenA );

					// Randomly generate a gradient vector given the integer coordinates of the
					// input value.  This implementation generates a random number and uses it
					// as an index into a normalized-vector lookup table.
					typename M::Vector4I	vectorIndexV = M::multiply( seedNoiseGenV, seedV );
					vectorIndexV = M::add( vectorIndexV, M::multiply( xNoiseGenV, ixV ) );

					vectorIndexV = M::bitXor( vectorIndexV, M::shiftRightLogical( vectorIndexV, SHIFT_NOISE_GEN ) );
					vectorIndexV = M::bitAnd( vectorIndexV, M::vectorizeOne( 0xff ) );
					vectorIndexV = M::multiply( vectorIndexV, M::constFourI() );

					VECTOR4_ALIGN( uint32	vectorIndexA[ 4 ] );
					M::storeToMemory( vectorIndexA, vectorIndexV );

					typename M::Vector4F	gx1x2____ = M::interleaveLo( M::loadFromMemory( Table::values() + vectorIndexA[ 0 ] ),
																		 M::loadFromMemory( Table::values() + vectorIndexA[ 1 ] ) );
					typename M::Vector4F	gx3x4____ = M::interleaveLo( M::loadFromMemory( Table::values() + vectorIndexA[ 2 ] ),
																		 M::loadFromMemory( Table::values() + vectorIndexA[ 3 ] ) );
					typename M::Vector4F	xvGradientV = M::template shuffle< 0, 1, 0, 1 >( gx1x2____, gx3x4____ );
					typename M::Vector4F	xvPointV = M::subtract( fxV, M::intToFloat( ixV ) );

					return M::multiply( M::multiply( xvGradientV, xvPointV ), M::vectorizeOne( ValueType( 2.12 ) ) );
				}

			};



			template< typename ValueT >
			class NoiseGenImpl< ValueT, 2, 4 >
			{

				public:
				
				typedef ValueT									ValueType;
				static const unsigned int						Dimension = 2;
				typedef math::Math< ValueType >					M;
				typedef math::Interpolations< ValueType >		Interp;
				typedef VectorTable< ValueType >				Table;



				public:
				
				static
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

					typename M::Vector4F		n1 = M::template shuffle< 2, 3, 0, 1 >( n, n );
					
					interpV = M::distribute1st( sCoordV );
					typename M::Vector4F		nx = Interp::LinearInterpV( n, n1, interpV );
					typename M::Vector4F		nx1 = M::template shuffle< 1, 0, 1, 0 >( nx, nx );
					
					interpV = M::distribute2nd( sCoordV );
					typename M::Vector4F		ny = Interp::LinearInterpV( nx, nx1, interpV );

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
				static
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



			public:

				static inline
				typename M::Vector4F
				GradientCoherentNoise4( const typename M::Vector4F& xV, const typename M::Vector4F& yV,
										const typename M::Vector4I& seedV, NoiseQuality noiseQuality = QUALITY_STD )
				{
					// Create a unit-length cube aligned along an integer boundary.  This cube
					// surrounds the input point.
					typename M::Vector4I	x0V = M::floatToIntTruncated( xV );
					x0V = M::subtract( x0V, M::signToOne( xV ) );
					typename M::Vector4I	x1V = M::add( x0V, M::constOneI() );
					typename M::Vector4I	y0V = M::floatToIntTruncated( yV );
					y0V = M::subtract( y0V, M::signToOne( yV ) );
					typename M::Vector4I	y1V = M::add( y0V, M::constOneI() );

					// Map the difference between the coordinates of the input value and the
					// coordinates of the cube's outer-lower-left vertex onto an S-curve.
					typename M::Vector4F	xsV = M::subtract( xV, M::intToFloat( x0V ) );
					typename M::Vector4F	ysV = M::subtract( yV, M::intToFloat( y0V ) );
					
					switch( noiseQuality )
					{
						case QUALITY_FAST:

						// do nothing more
						
						break;


						case QUALITY_STD:

						xsV = Interp::SCurve3V( xsV );
						ysV = Interp::SCurve3V( ysV );
						
						break;


						case QUALITY_BEST:

						xsV = Interp::SCurve5V( xsV );
						ysV = Interp::SCurve5V( ysV );
						
						break;
					}

					// Now calculate the noise values at each vertex of the cube.  To generate
					// the coherent-noise value at the input point, interpolate these eight
					// noise values using the S-curve value as the interpolant (trilinear
					// interpolation.)
					typename M::Vector4F	n0V;
					typename M::Vector4F	n1V;
					typename M::Vector4F	ix0V;
					typename M::Vector4F	ix1V;
					
					n0V = GradientNoise4( xV, yV, x0V, y0V, seedV );
					n1V = GradientNoise4( xV, yV, x1V, y0V, seedV );
					ix0V = Interp::LinearInterpV( n0V, n1V, xsV );

					n0V = GradientNoise4( xV, yV, x0V, y1V, seedV );
					n1V = GradientNoise4( xV, yV, x1V, y1V, seedV );
					ix1V = Interp::LinearInterpV( n0V, n1V, xsV );

					return Interp::LinearInterpV( ix0V, ix1V, ysV );
				}



			private:
				
				static inline
				typename M::Vector4F
				GradientNoise4( const typename M::Vector4F& fxV, const typename M::Vector4F& fyV,
								const typename M::Vector4I& ixV, const typename M::Vector4I& iyV,
								const typename M::Vector4I& seedV )
				{
					static VECTOR4_ALIGN( uint32	seedNoiseGenA[ 4 ] ) = { SEED_NOISE_GEN, SEED_NOISE_GEN, SEED_NOISE_GEN, SEED_NOISE_GEN };
					static VECTOR4_ALIGN( uint32	xNoiseGenA[ 4 ] ) = { X_NOISE_GEN, X_NOISE_GEN, X_NOISE_GEN, X_NOISE_GEN };
					static VECTOR4_ALIGN( uint32	yNoiseGenA[ 4 ] ) = { Y_NOISE_GEN, Y_NOISE_GEN, Y_NOISE_GEN, Y_NOISE_GEN };
					typename M::Vector4I	seedNoiseGenV = M::loadFromMemory( seedNoiseGenA );
					typename M::Vector4I	xNoiseGenV = M::loadFromMemory( xNoiseGenA );
					typename M::Vector4I	yNoiseGenV = M::loadFromMemory( yNoiseGenA );

					// Randomly generate a gradient vector given the integer coordinates of the
					// input value.  This implementation generates a random number and uses it
					// as an index into a normalized-vector lookup table.
					typename M::Vector4I	vectorIndexV = M::multiply( seedNoiseGenV, seedV );
					vectorIndexV = M::add( vectorIndexV, M::multiply( xNoiseGenV, ixV ) );
					vectorIndexV = M::add( vectorIndexV, M::multiply( yNoiseGenV, iyV ) );

					vectorIndexV = M::bitXor( vectorIndexV, M::shiftRightLogical( vectorIndexV, SHIFT_NOISE_GEN ) );
					vectorIndexV = M::bitAnd( vectorIndexV, M::vectorizeOne( 0xff ) );
					vectorIndexV = M::multiply( vectorIndexV, M::constFourI() );

					VECTOR4_ALIGN( uint32	vectorIndexA[ 4 ] );
					M::storeToMemory( vectorIndexA, vectorIndexV );

					typename M::Vector4F	gx1x2y1y2 = M::interleaveLo( M::loadFromMemory( Table::values() + vectorIndexA[ 0 ] ),
																		 M::loadFromMemory( Table::values() + vectorIndexA[ 1 ] ) );
					typename M::Vector4F	gx3x4y3y4 = M::interleaveLo( M::loadFromMemory( Table::values() + vectorIndexA[ 2 ] ),
																		 M::loadFromMemory( Table::values() + vectorIndexA[ 3 ] ) );
					typename M::Vector4F	xvGradientV = M::template shuffle< 0, 1, 0, 1 >( gx1x2y1y2, gx3x4y3y4 );
					typename M::Vector4F	yvGradientV = M::template shuffle< 2, 3, 2, 3 >( gx1x2y1y2, gx3x4y3y4 );

					typename M::Vector4F	xvPointV = M::subtract( fxV, M::intToFloat( ixV ) );
					typename M::Vector4F	yvPointV = M::subtract( fyV, M::intToFloat( iyV ) );

					typename M::Vector4F	nV = M::multiply( xvGradientV, xvPointV );
					nV = M::add( nV, M::multiply( yvGradientV, yvPointV ) );
					return M::multiply( nV, M::vectorizeOne( ValueType( 2.12 ) ) );
				}

			};



			template< typename ValueT >
			class NoiseGenImpl< ValueT, 3, 4 >
			{

				public:
				
				typedef ValueT									ValueType;
				static const unsigned int						Dimension = 3;
				typedef math::Math< ValueType >					M;
				typedef math::Interpolations< ValueType >		Interp;
				typedef VectorTable< ValueType >				Table;



				public:
				
				static
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
					typename M::Vector4F		nx1 = M::template shuffle< 2, 3, 0, 1 >( nx, nx );
					
					interpV = M::distribute2nd( sCoordV );
					typename M::Vector4F		ny = Interp::LinearInterpV( nx, nx1, interpV );
					typename M::Vector4F		ny1 = M::template shuffle< 1, 0, 1, 0 >( ny, ny );
					
					interpV = M::distribute3rd( sCoordV );
					typename M::Vector4F		nz = Interp::LinearInterpV( ny, ny1, interpV );

					return M::extract1st( nz );
				}



			private:

				struct Shuffle_0_3
				{
					typedef typename M::template Order< 0, 1, 0, 1 >		Shuffle_00;
					typedef typename M::template Order< 0, 1, 2, 1 >		Shuffle_01;
					typedef typename M::template Order< 0, 3, 0, 1 >		Shuffle_10;
					typedef typename M::template Order< 0, 3, 2, 1 >		Shuffle_11;
				};

				struct Shuffle_4_7
				{
					typedef typename M::template Order< 2, 1, 0, 1 >		Shuffle_00;
					typedef typename M::template Order< 2, 1, 2, 1 >		Shuffle_01;
					typedef typename M::template Order< 2, 3, 0, 1 >		Shuffle_10;
					typedef typename M::template Order< 2, 3, 2, 1 >		Shuffle_11;
				};



				template< typename ShufflePolicy >
				static
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



			public:

				static inline
				typename M::Vector4F
				GradientCoherentNoise4( const typename M::Vector4F& xV, const typename M::Vector4F& yV,
										const typename M::Vector4F& zV,
										const typename M::Vector4I& seedV, NoiseQuality noiseQuality = QUALITY_STD )
				{
					// Create a unit-length cube aligned along an integer boundary.  This cube
					// surrounds the input point.
					typename M::Vector4I	x0V = M::floatToIntTruncated( xV );
					x0V = M::subtract( x0V, M::signToOne( xV ) );
					typename M::Vector4I	x1V = M::add( x0V, M::constOneI() );
					typename M::Vector4I	y0V = M::floatToIntTruncated( yV );
					y0V = M::subtract( y0V, M::signToOne( yV ) );
					typename M::Vector4I	y1V = M::add( y0V, M::constOneI() );
					typename M::Vector4I	z0V = M::floatToIntTruncated( zV );
					z0V = M::subtract( z0V, M::signToOne( zV ) );
					typename M::Vector4I	z1V = M::add( z0V, M::constOneI() );

					// Map the difference between the coordinates of the input value and the
					// coordinates of the cube's outer-lower-left vertex onto an S-curve.
					typename M::Vector4F	xsV = M::subtract( xV, M::intToFloat( x0V ) );
					typename M::Vector4F	ysV = M::subtract( yV, M::intToFloat( y0V ) );
					typename M::Vector4F	zsV = M::subtract( zV, M::intToFloat( z0V ) );
					
					switch( noiseQuality )
					{
						case QUALITY_FAST:

						// do nothing more
						
						break;


						case QUALITY_STD:

						xsV = Interp::SCurve3V( xsV );
						ysV = Interp::SCurve3V( ysV );
						zsV = Interp::SCurve3V( zsV );
						
						break;


						case QUALITY_BEST:

						xsV = Interp::SCurve5V( xsV );
						ysV = Interp::SCurve5V( ysV );
						zsV = Interp::SCurve5V( zsV );
						
						break;
					}

					// Now calculate the noise values at each vertex of the cube.  To generate
					// the coherent-noise value at the input point, interpolate these eight
					// noise values using the S-curve value as the interpolant (trilinear
					// interpolation.)
					typename M::Vector4F	n0V;
					typename M::Vector4F	n1V;
					typename M::Vector4F	ix0V;
					typename M::Vector4F	ix1V;
					typename M::Vector4F	iy0V;
					typename M::Vector4F	iy1V;
					
					n0V = GradientNoise4( xV, yV, zV, x0V, y0V, z0V, seedV );
					n1V = GradientNoise4( xV, yV, zV, x1V, y0V, z0V, seedV );
					ix0V = Interp::LinearInterpV( n0V, n1V, xsV );
					n0V = GradientNoise4( xV, yV, zV, x0V, y1V, z0V, seedV );
					n1V = GradientNoise4( xV, yV, zV, x1V, y1V, z0V, seedV );
					ix1V = Interp::LinearInterpV( n0V, n1V, xsV );
					iy0V = Interp::LinearInterpV( ix0V, ix1V, ysV );

					n0V = GradientNoise4( xV, yV, zV, x0V, y0V, z1V, seedV );
					n1V = GradientNoise4( xV, yV, zV, x1V, y0V, z1V, seedV );
					ix0V = Interp::LinearInterpV( n0V, n1V, xsV );
					n0V = GradientNoise4( xV, yV, zV, x0V, y1V, z1V, seedV );
					n1V = GradientNoise4( xV, yV, zV, x1V, y1V, z1V, seedV );
					ix1V = Interp::LinearInterpV( n0V, n1V, xsV );
					iy1V = Interp::LinearInterpV( ix0V, ix1V, ysV );

					return Interp::LinearInterpV( iy0V, iy1V, zsV );
				}



			private:
				
				static inline
				typename M::Vector4F
				GradientNoise4( const typename M::Vector4F& fxV, const typename M::Vector4F& fyV,
								const typename M::Vector4F& fzV,
								const typename M::Vector4I& ixV, const typename M::Vector4I& iyV,
								const typename M::Vector4I& izV,
								const typename M::Vector4I& seedV )
				{
					static VECTOR4_ALIGN( uint32	seedNoiseGenA[ 4 ] ) = { SEED_NOISE_GEN, SEED_NOISE_GEN, SEED_NOISE_GEN, SEED_NOISE_GEN };
					static VECTOR4_ALIGN( uint32	xNoiseGenA[ 4 ] ) = { X_NOISE_GEN, X_NOISE_GEN, X_NOISE_GEN, X_NOISE_GEN };
					static VECTOR4_ALIGN( uint32	yNoiseGenA[ 4 ] ) = { Y_NOISE_GEN, Y_NOISE_GEN, Y_NOISE_GEN, Y_NOISE_GEN };
					static VECTOR4_ALIGN( uint32	zNoiseGenA[ 4 ] ) = { Z_NOISE_GEN, Z_NOISE_GEN, Z_NOISE_GEN, Z_NOISE_GEN };
					typename M::Vector4I	seedNoiseGenV = M::loadFromMemory( seedNoiseGenA );
					typename M::Vector4I	xNoiseGenV = M::loadFromMemory( xNoiseGenA );
					typename M::Vector4I	yNoiseGenV = M::loadFromMemory( yNoiseGenA );
					typename M::Vector4I	zNoiseGenV = M::loadFromMemory( zNoiseGenA );

					// Randomly generate a gradient vector given the integer coordinates of the
					// input value.  This implementation generates a random number and uses it
					// as an index into a normalized-vector lookup table.
					typename M::Vector4I	vectorIndexV = M::multiply( seedNoiseGenV, seedV );
					vectorIndexV = M::add( vectorIndexV, M::multiply( xNoiseGenV, ixV ) );
					vectorIndexV = M::add( vectorIndexV, M::multiply( yNoiseGenV, iyV ) );
					vectorIndexV = M::add( vectorIndexV, M::multiply( zNoiseGenV, izV ) );

					vectorIndexV = M::bitXor( vectorIndexV, M::shiftRightLogical( vectorIndexV, SHIFT_NOISE_GEN ) );
					vectorIndexV = M::bitAnd( vectorIndexV, M::vectorizeOne( 0xff ) );
					vectorIndexV = M::multiply( vectorIndexV, M::constFourI() );

					VECTOR4_ALIGN( uint32	vectorIndexA[ 4 ] );
					M::storeToMemory( vectorIndexA, vectorIndexV );

					typename M::Vector4F	g1 = M::loadFromMemory( Table::values() + vectorIndexA[ 0 ] );
					typename M::Vector4F	g2 = M::loadFromMemory( Table::values() + vectorIndexA[ 1 ] );
					typename M::Vector4F	g3 = M::loadFromMemory( Table::values() + vectorIndexA[ 2 ] );
					typename M::Vector4F	g4 = M::loadFromMemory( Table::values() + vectorIndexA[ 3 ] );
					typename M::Vector4F	gx1x2y1y2 = M::interleaveLo( g1, g2 );
					typename M::Vector4F	gx3x4y3y4 = M::interleaveLo( g3, g4 );
					typename M::Vector4F	xvGradientV = M::template shuffle< 0, 1, 0, 1 >( gx1x2y1y2, gx3x4y3y4 );
					typename M::Vector4F	yvGradientV = M::template shuffle< 2, 3, 2, 3 >( gx1x2y1y2, gx3x4y3y4 );
					typename M::Vector4F	gz1z2____ = M::interleaveHi( g1, g2 );
					typename M::Vector4F	gz3z4____ = M::interleaveHi( g3, g4 );
					typename M::Vector4F	zvGradientV = M::template shuffle< 0, 1, 0, 1 >( gz1z2____, gz3z4____ );

					typename M::Vector4F	xvPointV = M::subtract( fxV, M::intToFloat( ixV ) );
					typename M::Vector4F	yvPointV = M::subtract( fyV, M::intToFloat( iyV ) );
					typename M::Vector4F	zvPointV = M::subtract( fzV, M::intToFloat( izV ) );

					typename M::Vector4F	nV = M::multiply( xvGradientV, xvPointV );
					nV = M::add( nV, M::multiply( yvGradientV, yvPointV ) );
					nV = M::add( nV, M::multiply( zvGradientV, zvPointV ) );
					return M::multiply( nV, M::vectorizeOne( ValueType( 2.12 ) ) );
				}

			};



			template< typename ValueT >
			class NoiseGenImpl< ValueT, 4, 4 >
			{

				public:
				
				typedef ValueT									ValueType;
				static const unsigned int						Dimension = 4;
				typedef math::Math< ValueType >					M;
				typedef math::Interpolations< ValueType >		Interp;
				typedef VectorTable< ValueType >				Table;



				public:
				
				static
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
					typename M::Vector4F		ny1 = M::template shuffle< 2, 3, 0, 1 >( ny, ny );
					
					interpV = M::distribute3rd( sCoordV );
					typename M::Vector4F		nz = Interp::LinearInterpV( ny, ny1, interpV );
					typename M::Vector4F		nz1 = M::template shuffle< 1, 0, 1, 0 >( nz, nz );
					
					interpV = M::distribute4th( sCoordV );
					typename M::Vector4F		nw = Interp::LinearInterpV( nz, nz1, interpV );

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
				static
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



			public:

				static inline
				typename M::Vector4F
				GradientCoherentNoise4( const typename M::Vector4F& xV, const typename M::Vector4F& yV,
										const typename M::Vector4F& zV, const typename M::Vector4F& wV,
										const typename M::Vector4I& seedV, NoiseQuality noiseQuality = QUALITY_STD )
				{
					// Create a unit-length cube aligned along an integer boundary.  This cube
					// surrounds the input point.
					typename M::Vector4I	x0V = M::floatToIntTruncated( xV );
					x0V = M::subtract( x0V, M::signToOne( xV ) );
					typename M::Vector4I	x1V = M::add( x0V, M::constOneI() );
					typename M::Vector4I	y0V = M::floatToIntTruncated( yV );
					y0V = M::subtract( y0V, M::signToOne( yV ) );
					typename M::Vector4I	y1V = M::add( y0V, M::constOneI() );
					typename M::Vector4I	z0V = M::floatToIntTruncated( zV );
					z0V = M::subtract( z0V, M::signToOne( zV ) );
					typename M::Vector4I	z1V = M::add( z0V, M::constOneI() );
					typename M::Vector4I	w0V = M::floatToIntTruncated( wV );
					w0V = M::subtract( w0V, M::signToOne( wV ) );
					typename M::Vector4I	w1V = M::add( w0V, M::constOneI() );

					// Map the difference between the coordinates of the input value and the
					// coordinates of the cube's outer-lower-left vertex onto an S-curve.
					typename M::Vector4F	xsV = M::subtract( xV, M::intToFloat( x0V ) );
					typename M::Vector4F	ysV = M::subtract( yV, M::intToFloat( y0V ) );
					typename M::Vector4F	zsV = M::subtract( zV, M::intToFloat( z0V ) );
					typename M::Vector4F	wsV = M::subtract( wV, M::intToFloat( w0V ) );
					
					switch( noiseQuality )
					{
						case QUALITY_FAST:

						// do nothing more
						
						break;


						case QUALITY_STD:

						xsV = Interp::SCurve3V( xsV );
						ysV = Interp::SCurve3V( ysV );
						zsV = Interp::SCurve3V( zsV );
						wsV = Interp::SCurve3V( wsV );
						
						break;


						case QUALITY_BEST:

						xsV = Interp::SCurve5V( xsV );
						ysV = Interp::SCurve5V( ysV );
						zsV = Interp::SCurve5V( zsV );
						wsV = Interp::SCurve5V( wsV );
						
						break;
					}

					// Now calculate the noise values at each vertex of the cube.  To generate
					// the coherent-noise value at the input point, interpolate these eight
					// noise values using the S-curve value as the interpolant (trilinear
					// interpolation.)
					typename M::Vector4F	n0V;
					typename M::Vector4F	n1V;
					typename M::Vector4F	ix0V;
					typename M::Vector4F	ix1V;
					typename M::Vector4F	iy0V;
					typename M::Vector4F	iy1V;
					typename M::Vector4F	iz0V;
					typename M::Vector4F	iz1V;
					
					n0V = GradientNoise4( xV, yV, zV, wV, x0V, y0V, z0V, w0V, seedV );
					n1V = GradientNoise4( xV, yV, zV, wV, x1V, y0V, z0V, w0V, seedV );
					ix0V = Interp::LinearInterpV( n0V, n1V, xsV );
					n0V = GradientNoise4( xV, yV, zV, wV, x0V, y1V, z0V, w0V, seedV );
					n1V = GradientNoise4( xV, yV, zV, wV, x1V, y1V, z0V, w0V, seedV );
					ix1V = Interp::LinearInterpV( n0V, n1V, xsV );
					iy0V = Interp::LinearInterpV( ix0V, ix1V, ysV );

					n0V = GradientNoise4( xV, yV, zV, wV, x0V, y0V, z1V, w0V, seedV );
					n1V = GradientNoise4( xV, yV, zV, wV, x1V, y0V, z1V, w0V, seedV );
					ix0V = Interp::LinearInterpV( n0V, n1V, xsV );
					n0V = GradientNoise4( xV, yV, zV, wV, x0V, y1V, z1V, w0V, seedV );
					n1V = GradientNoise4( xV, yV, zV, wV, x1V, y1V, z1V, w0V, seedV );
					ix1V = Interp::LinearInterpV( n0V, n1V, xsV );
					iy1V = Interp::LinearInterpV( ix0V, ix1V, ysV );
					iz0V = Interp::LinearInterpV( iy0V, iy1V, zsV );

					n0V = GradientNoise4( xV, yV, zV, wV, x0V, y0V, z0V, w1V, seedV );
					n1V = GradientNoise4( xV, yV, zV, wV, x1V, y0V, z0V, w1V, seedV );
					ix0V = Interp::LinearInterpV( n0V, n1V, xsV );
					n0V = GradientNoise4( xV, yV, zV, wV, x0V, y1V, z0V, w1V, seedV );
					n1V = GradientNoise4( xV, yV, zV, wV, x1V, y1V, z0V, w1V, seedV );
					ix1V = Interp::LinearInterpV( n0V, n1V, xsV );
					iy0V = Interp::LinearInterpV( ix0V, ix1V, ysV );

					n0V = GradientNoise4( xV, yV, zV, wV, x0V, y0V, z1V, w1V, seedV );
					n1V = GradientNoise4( xV, yV, zV, wV, x1V, y0V, z1V, w1V, seedV );
					ix0V = Interp::LinearInterpV( n0V, n1V, xsV );
					n0V = GradientNoise4( xV, yV, zV, wV, x0V, y1V, z1V, w1V, seedV );
					n1V = GradientNoise4( xV, yV, zV, wV, x1V, y1V, z1V, w1V, seedV );
					ix1V = Interp::LinearInterpV( n0V, n1V, xsV );
					iy1V = Interp::LinearInterpV( ix0V, ix1V, ysV );
					iz1V = Interp::LinearInterpV( iy0V, iy1V, zsV );

					return Interp::LinearInterpV( iz0V, iz1V, wsV );
				}



			private:
				
				static inline
				typename M::Vector4F
				GradientNoise4( const typename M::Vector4F& fxV, const typename M::Vector4F& fyV,
								const typename M::Vector4F& fzV, const typename M::Vector4F& fwV,
								const typename M::Vector4I& ixV, const typename M::Vector4I& iyV,
								const typename M::Vector4I& izV, const typename M::Vector4I& iwV,
								const typename M::Vector4I& seedV )
				{
					static VECTOR4_ALIGN( uint32	seedNoiseGenA[ 4 ] ) = { SEED_NOISE_GEN, SEED_NOISE_GEN, SEED_NOISE_GEN, SEED_NOISE_GEN };
					static VECTOR4_ALIGN( uint32	xNoiseGenA[ 4 ] ) = { X_NOISE_GEN, X_NOISE_GEN, X_NOISE_GEN, X_NOISE_GEN };
					static VECTOR4_ALIGN( uint32	yNoiseGenA[ 4 ] ) = { Y_NOISE_GEN, Y_NOISE_GEN, Y_NOISE_GEN, Y_NOISE_GEN };
					static VECTOR4_ALIGN( uint32	zNoiseGenA[ 4 ] ) = { Z_NOISE_GEN, Z_NOISE_GEN, Z_NOISE_GEN, Z_NOISE_GEN };
					static VECTOR4_ALIGN( uint32	wNoiseGenA[ 4 ] ) = { W_NOISE_GEN, W_NOISE_GEN, W_NOISE_GEN, W_NOISE_GEN };
					typename M::Vector4I	seedNoiseGenV = M::loadFromMemory( seedNoiseGenA );
					typename M::Vector4I	xNoiseGenV = M::loadFromMemory( xNoiseGenA );
					typename M::Vector4I	yNoiseGenV = M::loadFromMemory( yNoiseGenA );
					typename M::Vector4I	zNoiseGenV = M::loadFromMemory( zNoiseGenA );
					typename M::Vector4I	wNoiseGenV = M::loadFromMemory( wNoiseGenA );

					// Randomly generate a gradient vector given the integer coordinates of the
					// input value.  This implementation generates a random number and uses it
					// as an index into a normalized-vector lookup table.
					typename M::Vector4I	vectorIndexV = M::multiply( seedNoiseGenV, seedV );
					vectorIndexV = M::add( vectorIndexV, M::multiply( xNoiseGenV, ixV ) );
					vectorIndexV = M::add( vectorIndexV, M::multiply( yNoiseGenV, iyV ) );
					vectorIndexV = M::add( vectorIndexV, M::multiply( zNoiseGenV, izV ) );
					vectorIndexV = M::add( vectorIndexV, M::multiply( wNoiseGenV, iwV ) );

					vectorIndexV = M::bitXor( vectorIndexV, M::shiftRightLogical( vectorIndexV, SHIFT_NOISE_GEN ) );
					vectorIndexV = M::bitAnd( vectorIndexV, M::vectorizeOne( 0xff ) );
					vectorIndexV = M::multiply( vectorIndexV, M::constFourI() );

					VECTOR4_ALIGN( uint32	vectorIndexA[ 4 ] );
					M::storeToMemory( vectorIndexA, vectorIndexV );

					typename M::Vector4F	g1 = M::loadFromMemory( Table::values() + vectorIndexA[ 0 ] );
					typename M::Vector4F	g2 = M::loadFromMemory( Table::values() + vectorIndexA[ 1 ] );
					typename M::Vector4F	g3 = M::loadFromMemory( Table::values() + vectorIndexA[ 2 ] );
					typename M::Vector4F	g4 = M::loadFromMemory( Table::values() + vectorIndexA[ 3 ] );
					typename M::Vector4F	gx1x2y1y2 = M::interleaveLo( g1, g2 );
					typename M::Vector4F	gx3x4y3y4 = M::interleaveLo( g3, g4 );
					typename M::Vector4F	xvGradientV = M::template shuffle< 0, 1, 0, 1 >( gx1x2y1y2, gx3x4y3y4 );
					typename M::Vector4F	yvGradientV = M::template shuffle< 2, 3, 2, 3 >( gx1x2y1y2, gx3x4y3y4 );
					typename M::Vector4F	gz1z2w1w2 = M::interleaveHi( g1, g2 );
					typename M::Vector4F	gz3z4w3w4 = M::interleaveHi( g3, g4 );
					typename M::Vector4F	zvGradientV = M::template shuffle< 0, 1, 0, 1 >( gz1z2w1w2, gz3z4w3w4 );
					typename M::Vector4F	wvGradientV = M::template shuffle< 2, 3, 2, 3 >( gz1z2w1w2, gz3z4w3w4 );

					typename M::Vector4F	xvPointV = M::subtract( fxV, M::intToFloat( ixV ) );
					typename M::Vector4F	yvPointV = M::subtract( fyV, M::intToFloat( iyV ) );
					typename M::Vector4F	zvPointV = M::subtract( fzV, M::intToFloat( izV ) );
					typename M::Vector4F	wvPointV = M::subtract( fwV, M::intToFloat( iwV ) );

					typename M::Vector4F	nV = M::multiply( xvGradientV, xvPointV );
					nV = M::add( nV, M::multiply( yvGradientV, yvPointV ) );
					nV = M::add( nV, M::multiply( zvGradientV, zvPointV ) );
					nV = M::add( nV, M::multiply( wvGradientV, wvPointV ) );
					return M::multiply( nV, M::vectorizeOne( ValueType( 2.12 ) ) );
				}

			};

		}

	}

}