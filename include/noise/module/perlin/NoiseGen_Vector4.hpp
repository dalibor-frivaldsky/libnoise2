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

					//typename M::Vector4I		coord0V = M::floatToIntTruncated( xyz_0 );
					//typename M::Vector4I		coord1V = M::add( coord0V, M::constOneI() );

					VECTOR4_ALIGN( int	cubeBoundariesVector[ 4 ] );
					M::storeToMemory( cubeBoundariesVector, coord0V );
					int	x0 = cubeBoundariesVector[ 0 ];
					int	y0 = cubeBoundariesVector[ 1 ];
					int	z0 = cubeBoundariesVector[ 2 ];
					
					M::storeToMemory( cubeBoundariesVector, coord1V );
					int	x1 = cubeBoundariesVector[ 0 ];
					int	y1 = cubeBoundariesVector[ 1 ];
					int	z1 = cubeBoundariesVector[ 2 ];
					
					// Map the difference between the coordinates of the input value and the
					// coordinates of the cube's outer-lower-left vertex onto an S-curve.
					typename M::Vector4F		sCoordV;
					typename M::Vector4F		xyz_0 = M::intToFloat( coord0V );
					typename M::Vector4F		xyz_1 = M::intToFloat( coord1V );
					
					// Calculate distance from the "zero" cube coordinates
					sCoordV = M::subtract( coordV, xyz_0 );
					
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
					
					// Now calculate the noise values at each vertex of the cube.  To generate
					// the coherent-noise value at the input point, interpolate these eight
					// noise values using the S-curve value as the interpolant (trilinear
					// interpolation.)
					
					typename M::Vector4F		x0y0x1y1 = M::template shuffle< 1, 0, 1, 0 >( xyz_0, xyz_1 );
					typename M::Vector4F		z0__z1__ = M::template shuffle< 3, 2, 3, 2 >( xyz_0, xyz_1 );
					
					int							indexBase;
					typename M::Vector4F		interpV;
					typename M::Vector4F		n1;
					typename M::Vector4F		n2;

					indexBase = SEED_NOISE_GEN * seed + X_NOISE_GEN * x0;
					calculateNoisePart< Shuffle_0_3 >( coordV, x0y0x1y1, z0__z1__, indexBase, y0, y1, z0, z1, n1 );

					indexBase = SEED_NOISE_GEN * seed + X_NOISE_GEN * x1;
					calculateNoisePart< Shuffle_4_7 >( coordV, x0y0x1y1, z0__z1__, indexBase, y0, y1, z0, z1, n2 );

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
					typedef typename M::template Order< 1, 0, 1, 0 >		Shuffle_00;
					typedef typename M::template Order< 1, 0, 3, 0 >		Shuffle_01;
					typedef typename M::template Order< 1, 2, 1, 0 >		Shuffle_10;
					typedef typename M::template Order< 1, 2, 3, 0 >		Shuffle_11;
				};

				struct Shuffle_4_7
				{
					typedef typename M::template Order< 3, 0, 1, 0 >		Shuffle_00;
					typedef typename M::template Order< 3, 0, 3, 0 >		Shuffle_01;
					typedef typename M::template Order< 3, 2, 1, 0 >		Shuffle_10;
					typedef typename M::template Order< 3, 2, 3, 0 >		Shuffle_11;
				};



				template< typename ShufflePolicy >
				static inline
				void
				calculateNoisePart( typename M::Vector4F& coordV, typename M::Vector4F& x0y0x1y1, typename M::Vector4F& z0w0z1w1,
									int indexBase, int iz0, int iz1, int iw0, int iw1,
									typename M::Vector4F& n0 )
				{
					// define local SSE array and vector arrays
					ValueType*		tableV = Table::values();
					
					// multiply with gradients
					int				tableIndex;
					typename M::Vector4F		gradient;

					typename M::Vector4F		n1 = M::template shuffle< typename ShufflePolicy::Shuffle_00 >( x0y0x1y1, z0w0z1w1 );
					n1 = M::subtract( coordV, n1 );
					tableIndex = indexBase + Y_NOISE_GEN * iz0 + Z_NOISE_GEN * iw0;
					tableIndex ^= (tableIndex >> SHIFT_NOISE_GEN);
					tableIndex &= 0xff;
					gradient = M::loadFromMemory( tableV + tableIndex * 4 );
					n1 = M::multiply( n1, gradient );
					
					typename M::Vector4F		n2 = M::template shuffle< typename ShufflePolicy::Shuffle_01 >( x0y0x1y1, z0w0z1w1 );
					n2 = M::subtract( coordV, n2 );
					tableIndex = indexBase + Y_NOISE_GEN * iz0 + Z_NOISE_GEN * iw1;
					tableIndex ^= (tableIndex >> SHIFT_NOISE_GEN);
					tableIndex &= 0xff;
					gradient = M::loadFromMemory( tableV + tableIndex * 4 );
					n2 = M::multiply( n2, gradient );

					typename M::Vector4F		nd1 = M::reduce( n1, n2 );

					n1 = M::template shuffle< typename ShufflePolicy::Shuffle_10 >( x0y0x1y1, z0w0z1w1 );
					n1 = M::subtract( coordV, n1 );
					tableIndex = indexBase + Y_NOISE_GEN * iz1 + Z_NOISE_GEN * iw0;
					tableIndex ^= (tableIndex >> SHIFT_NOISE_GEN);
					tableIndex &= 0xff;
					gradient = M::loadFromMemory( tableV + tableIndex * 4 );
					n1 = M::multiply( n1, gradient );

					n2 = M::template shuffle< typename ShufflePolicy::Shuffle_11 >( x0y0x1y1, z0w0z1w1 );
					n2 = M::subtract( coordV, n2 );
					tableIndex = indexBase + Y_NOISE_GEN * iz1 + Z_NOISE_GEN * iw1;
					tableIndex ^= (tableIndex >> SHIFT_NOISE_GEN);
					tableIndex &= 0xff;
					gradient = M::loadFromMemory( tableV + tableIndex * 4 );
					n2 = M::multiply( n2, gradient );

					typename M::Vector4F		nd2 = M::reduce( n1, n2 );
					typename M::Vector4F		nd = M::reduce( nd1, nd2 );
					
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

					VECTOR4_ALIGN( int	cubeBoundariesVector[ 4 ] );
					M::storeToMemory( cubeBoundariesVector, coord0V );
					int	x0 = cubeBoundariesVector[ 0 ];
					int	y0 = cubeBoundariesVector[ 1 ];
					int	z0 = cubeBoundariesVector[ 2 ];
					int	w0 = cubeBoundariesVector[ 3 ];

					M::storeToMemory( cubeBoundariesVector, coord1V );
					int	x1 = cubeBoundariesVector[ 0 ];
					int	y1 = cubeBoundariesVector[ 1 ];
					int	z1 = cubeBoundariesVector[ 2 ];
					int	w1 = cubeBoundariesVector[ 3 ];

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
					
					// Now calculate the noise values at each vertex of the cube.  To generate
					// the coherent-noise value at the input point, interpolate these eight
					// noise values using the S-curve value as the interpolant (trilinear
					// interpolation.)
					
					typename M::Vector4F		x0y0x1y1 = M::template shuffle< 1, 0, 1, 0 >( xyzw0, xyzw1 );
					typename M::Vector4F		z0w0z1w1 = M::template shuffle< 3, 2, 3, 2 >( xyzw0, xyzw1 );

					int							indexBase;
					typename M::Vector4F		interpV;
					typename M::Vector4F		n1;
					typename M::Vector4F		n2;

					indexBase = SEED_NOISE_GEN * seed + X_NOISE_GEN * x0 + Y_NOISE_GEN * y0;
					calculateNoisePart< Shuffle_0_3 >( coordV, x0y0x1y1, z0w0z1w1, indexBase, z0, z1, w0, w1, n1 );

					indexBase = SEED_NOISE_GEN * seed + X_NOISE_GEN * x1 + Y_NOISE_GEN * y0;
					calculateNoisePart< Shuffle_8_11 >( coordV, x0y0x1y1, z0w0z1w1, indexBase, z0, z1, w0, w1, n2 );

					interpV = M::distribute1st( sCoordV );
					typename M::Vector4F		nx1 = Interp::LinearInterpV( n1, n2, interpV );

					indexBase = SEED_NOISE_GEN * seed + X_NOISE_GEN * x0 + Y_NOISE_GEN * y1;
					calculateNoisePart< Shuffle_4_7 >( coordV, x0y0x1y1, z0w0z1w1, indexBase, z0, z1, w0, w1, n1 );

					indexBase = SEED_NOISE_GEN * seed + X_NOISE_GEN * x1 + Y_NOISE_GEN * y1;
					calculateNoisePart< Shuffle_12_15 >( coordV, x0y0x1y1, z0w0z1w1, indexBase, z0, z1, w0, w1, n2 );

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
					typedef typename M::template Order< 1, 0, 1, 0 >		Shuffle_00;
					typedef typename M::template Order< 1, 0, 1, 2 >		Shuffle_01;
					typedef typename M::template Order< 1, 0, 3, 0 >		Shuffle_10;
					typedef typename M::template Order< 1, 0, 3, 2 >		Shuffle_11;
				};

				struct Shuffle_4_7
				{
					typedef typename M::template Order< 1, 2, 1, 0 >		Shuffle_00;
					typedef typename M::template Order< 1, 2, 1, 2 >		Shuffle_01;
					typedef typename M::template Order< 1, 2, 3, 0 >		Shuffle_10;
					typedef typename M::template Order< 1, 2, 3, 2 >		Shuffle_11;
				};

				struct Shuffle_8_11
				{
					typedef typename M::template Order< 3, 0, 1, 0 >		Shuffle_00;
					typedef typename M::template Order< 3, 0, 1, 2 >		Shuffle_01;
					typedef typename M::template Order< 3, 0, 3, 0 >		Shuffle_10;
					typedef typename M::template Order< 3, 0, 3, 2 >		Shuffle_11;
				};

				struct Shuffle_12_15
				{
					typedef typename M::template Order< 3, 2, 1, 0 >		Shuffle_00;
					typedef typename M::template Order< 3, 2, 1, 2 >		Shuffle_01;
					typedef typename M::template Order< 3, 2, 3, 0 >		Shuffle_10;
					typedef typename M::template Order< 3, 2, 3, 2 >		Shuffle_11;
				};



				template< typename ShufflePolicy >
				static inline
				void
				calculateNoisePart( typename M::Vector4F& coordV, typename M::Vector4F& x0y0x1y1, typename M::Vector4F& z0w0z1w1,
									int indexBase, int iz0, int iz1, int iw0, int iw1,
									typename M::Vector4F& n0 )
				{
					// define local SSE array and vector arrays
					ValueType*		tableV = Table::values();
					
					// multiply with gradients
					int							tableIndex;
					typename M::Vector4F		gradient;

					typename M::Vector4F		n1 = M::template shuffle< typename ShufflePolicy::Shuffle_00 >( x0y0x1y1, z0w0z1w1 );
					tableIndex = indexBase + Z_NOISE_GEN * iz0 + W_NOISE_GEN * iw0;
					tableIndex ^= (tableIndex >> SHIFT_NOISE_GEN);
					tableIndex &= 0xff;
					gradient = M::loadFromMemory( tableV + tableIndex * 4 );
					n1 = M::multiply( n1, gradient );

					typename M::Vector4F		n2 = M::template shuffle< typename ShufflePolicy::Shuffle_01 >( x0y0x1y1, z0w0z1w1 );
					tableIndex = indexBase + Z_NOISE_GEN * iz0 + W_NOISE_GEN * iw1;
					tableIndex ^= (tableIndex >> SHIFT_NOISE_GEN);
					tableIndex &= 0xff;
					gradient = M::loadFromMemory( tableV + tableIndex * 4 );
					n2 = M::multiply( n2, gradient );

					typename M::Vector4F		nd1 = M::reduce( n1, n2 );
					
					n1 = M::template shuffle< typename ShufflePolicy::Shuffle_10 >( x0y0x1y1, z0w0z1w1 );
					tableIndex = indexBase + Z_NOISE_GEN * iz1 + W_NOISE_GEN * iw0;
					tableIndex ^= (tableIndex >> SHIFT_NOISE_GEN);
					tableIndex &= 0xff;
					gradient = M::loadFromMemory( tableV + tableIndex * 4 );
					n1 = M::multiply( n1, gradient );

					n2 = M::template shuffle< typename ShufflePolicy::Shuffle_11 >( x0y0x1y1, z0w0z1w1 );
					tableIndex = indexBase + Z_NOISE_GEN * iz1 + W_NOISE_GEN * iw1;
					tableIndex ^= (tableIndex >> SHIFT_NOISE_GEN);
					tableIndex &= 0xff;
					gradient = M::loadFromMemory( tableV + tableIndex * 4 );
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