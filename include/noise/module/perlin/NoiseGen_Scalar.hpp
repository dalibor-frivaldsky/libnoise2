#pragma once


// libnoise
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
			class NoiseGenImpl< ValueT, 1, 1 >
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
					int	x0 = ( x > ValueType( 0.0 ) ? (int) x: (int) x - 1 );
					int	x1 = x0 + 1;
					
					// Map the difference between the coordinates of the input value and the
					// coordinates of the cube's outer-lower-left vertex onto an S-curve.
					ValueType xs = ValueType( 0.0 );
					
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
					return xvGradient * xvPoint * ValueType( 2.12 );
				}

			};



			template< typename ValueT >
			class NoiseGenImpl< ValueT, 2, 1 >
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
				GradientCoherentNoise( ValueType x, ValueType y, int seed = 0, NoiseQuality noiseQuality = QUALITY_STD )
				{
					// Create a unit-length cube aligned along an integer boundary.  This cube
					// surrounds the input point.
					int	x0 = ( x > ValueType( 0.0 ) ? (int) x: (int) x - 1 );
					int	x1 = x0 + 1;
					int	y0 = ( y > ValueType( 0.0 ) ? (int) y: (int) y - 1 );
					int	y1 = y0 + 1;
					
					// Map the difference between the coordinates of the input value and the
					// coordinates of the cube's outer-lower-left vertex onto an S-curve.
					ValueType xs = ValueType( 0.0 ), ys = ValueType( 0.0 );
					
					switch( noiseQuality )
					{
						case QUALITY_FAST:

						xs = ( x - (ValueType) x0 );
						ys = ( y - (ValueType) y0 );
						
						break;


						case QUALITY_STD:

						xs = Interp::SCurve3( x - (ValueType) x0 );
						ys = Interp::SCurve3( y - (ValueType) y0 );
						
						break;


						case QUALITY_BEST:

						xs = Interp::SCurve5( x - (ValueType) x0 );
						ys = Interp::SCurve5( y - (ValueType) y0 );
						
						break;
					}

					// Now calculate the noise values at each vertex of the cube.  To generate
					// the coherent-noise value at the input point, interpolate these eight
					// noise values using the S-curve value as the interpolant (trilinear
					// interpolation.)
					ValueType n0, n1, ix0, ix1;

					n0   = GradientNoise( x, y, x0, y0, seed );
					n1   = GradientNoise( x, y, x1, y0, seed );
					ix0  = Interp::LinearInterp( n0, n1, xs );
					n0   = GradientNoise( x, y, x0, y1, seed );
					n1   = GradientNoise( x, y, x1, y1, seed );
					ix1  = Interp::LinearInterp( n0, n1, xs );

					return Interp::LinearInterp( ix0, ix1, ys );
				}
				
				static inline
				ValueType
				GradientNoise( ValueType fx, ValueType fy, int ix, int iy, int seed )
				{
					// Randomly generate a gradient vector given the integer coordinates of the
					// input value.  This implementation generates a random number and uses it
					// as an index into a normalized-vector lookup table.
					int vectorIndex = (
					  X_NOISE_GEN    * ix
					+ Y_NOISE_GEN    * iy
					+ SEED_NOISE_GEN * seed)
					& 0xffffffff;
					
					vectorIndex ^= (vectorIndex >> SHIFT_NOISE_GEN);
					vectorIndex &= 0xff;

					ValueType	xvGradient = Table::values()[ (vectorIndex << 2)     ];
					ValueType	yvGradient = Table::values()[ (vectorIndex << 2) + 1 ];
					
					// Set up us another vector equal to the distance between the two vectors
					// passed to this function.
					ValueType	xvPoint = (fx - (ValueType) ix);
					ValueType	yvPoint = (fy - (ValueType) iy);
					
					// Now compute the dot product of the gradient vector with the distance
					// vector.  The resulting value is gradient noise.  Apply a scaling value
					// so that this noise value ranges from -1.0 to 1.0.
					return ((xvGradient * xvPoint)
							+ (yvGradient * yvPoint)) * ValueType( 2.12 );
				}

			};



			template< typename ValueT >
			class NoiseGenImpl< ValueT, 3, 1 >
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
				GradientCoherentNoise( ValueType x, ValueType y, ValueType z, int seed = 0, NoiseQuality noiseQuality = QUALITY_STD )
				{
					// Create a unit-length cube aligned along an integer boundary.  This cube
					// surrounds the input point.
					int	x0 = ( x > ValueType( 0.0 ) ? (int) x: (int) x - 1 );
					int	x1 = x0 + 1;
					int	y0 = ( y > ValueType( 0.0 ) ? (int) y: (int) y - 1 );
					int	y1 = y0 + 1;
					int	z0 = ( z > ValueType( 0.0 ) ? (int) z: (int) z - 1 );
					int	z1 = z0 + 1;
					
					// Map the difference between the coordinates of the input value and the
					// coordinates of the cube's outer-lower-left vertex onto an S-curve.
					ValueType xs = ValueType( 0.0 ), ys = ValueType( 0.0 ), zs = ValueType( 0.0 );
					
					switch( noiseQuality )
					{
						case QUALITY_FAST:

						xs = ( x - (ValueType) x0 );
						ys = ( y - (ValueType) y0 );
						zs = ( z - (ValueType) z0 );
						
						break;


						case QUALITY_STD:

						xs = Interp::SCurve3( x - (ValueType) x0 );
						ys = Interp::SCurve3( y - (ValueType) y0 );
						zs = Interp::SCurve3( z - (ValueType) z0 );
						
						break;


						case QUALITY_BEST:

						xs = Interp::SCurve5( x - (ValueType) x0 );
						ys = Interp::SCurve5( y - (ValueType) y0 );
						zs = Interp::SCurve5( z - (ValueType) z0 );
						
						break;
					}

					// Now calculate the noise values at each vertex of the cube.  To generate
					// the coherent-noise value at the input point, interpolate these eight
					// noise values using the S-curve value as the interpolant (trilinear
					// interpolation.)
					ValueType n0, n1, ix0, ix1, iy0, iy1;

					n0   = GradientNoise( x, y, z, x0, y0, z0, seed );
					n1   = GradientNoise( x, y, z, x1, y0, z0, seed );
					ix0  = Interp::LinearInterp( n0, n1, xs );
					n0   = GradientNoise( x, y, z, x0, y1, z0, seed );
					n1   = GradientNoise( x, y, z, x1, y1, z0, seed );
					ix1  = Interp::LinearInterp( n0, n1, xs );
					iy0  = Interp::LinearInterp( ix0, ix1, ys );
					n0   = GradientNoise( x, y, z, x0, y0, z1, seed );
					n1   = GradientNoise( x, y, z, x1, y0, z1, seed );
					ix0  = Interp::LinearInterp( n0, n1, xs );
					n0   = GradientNoise( x, y, z, x0, y1, z1, seed );
					n1   = GradientNoise( x, y, z, x1, y1, z1, seed );
					ix1  = Interp::LinearInterp( n0, n1, xs );
					iy1  = Interp::LinearInterp( ix0, ix1, ys );
					
					return Interp::LinearInterp( iy0, iy1, zs );
				}
				
				static inline
				ValueType
				GradientNoise( ValueType fx, ValueType fy, ValueType fz, int ix, int iy, int iz, int seed )
				{
					// Randomly generate a gradient vector given the integer coordinates of the
					// input value.  This implementation generates a random number and uses it
					// as an index into a normalized-vector lookup table.
					int vectorIndex = (
					  X_NOISE_GEN    * ix
					+ Y_NOISE_GEN    * iy
					+ Z_NOISE_GEN    * iz
					+ SEED_NOISE_GEN * seed)
					& 0xffffffff;
					
					vectorIndex ^= (vectorIndex >> SHIFT_NOISE_GEN);
					vectorIndex &= 0xff;

					ValueType	xvGradient = Table::values()[ (vectorIndex << 2)     ];
					ValueType	yvGradient = Table::values()[ (vectorIndex << 2) + 1 ];
					ValueType	zvGradient = Table::values()[ (vectorIndex << 2) + 2 ];
					
					// Set up us another vector equal to the distance between the two vectors
					// passed to this function.
					ValueType	xvPoint = (fx - (ValueType) ix);
					ValueType	yvPoint = (fy - (ValueType) iy);
					ValueType	zvPoint = (fz - (ValueType) iz);
					
					// Now compute the dot product of the gradient vector with the distance
					// vector.  The resulting value is gradient noise.  Apply a scaling value
					// so that this noise value ranges from -1.0 to 1.0.
					return ((xvGradient * xvPoint)
							+ (yvGradient * yvPoint)
							+ (zvGradient * zvPoint)) * ValueType( 2.12 );
				}
				
			};



			template< typename ValueT >
			class NoiseGenImpl< ValueT, 4, 1 >
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
				GradientCoherentNoise( ValueType x, ValueType y, ValueType z, ValueType w, int seed = 0, NoiseQuality noiseQuality = QUALITY_STD )
				{
					// Create a unit-length cube aligned along an integer boundary.  This cube
					// surrounds the input point.
					int	x0 = ( x > ValueType( 0.0 ) ? (int) x: (int) x - 1 );
					int	x1 = x0 + 1;
					int	y0 = ( y > ValueType( 0.0 ) ? (int) y: (int) y - 1 );
					int	y1 = y0 + 1;
					int	z0 = ( z > ValueType( 0.0 ) ? (int) z: (int) z - 1 );
					int	z1 = z0 + 1;
					int	w0 = ( w > ValueType( 0.0 ) ? (int) w: (int) w - 1 );
					int	w1 = w0 + 1;

					// Map the difference between the coordinates of the input value and the
					// coordinates of the cube's outer-lower-left vertex onto an S-curve.
					ValueType xs = ValueType( 0.0 ), ys = ValueType( 0.0 ), zs = ValueType( 0.0 ), ws = ValueType( 0.0 );
					
					switch( noiseQuality )
					{
						case QUALITY_FAST:

						xs = ( x - (ValueType) x0 );
						ys = ( y - (ValueType) y0 );
						zs = ( z - (ValueType) z0 );
						ws = ( w - (ValueType) w0 );

						break;


						case QUALITY_STD:

						xs = Interp::SCurve3( x - (ValueType) x0 );
						ys = Interp::SCurve3( y - (ValueType) y0 );
						zs = Interp::SCurve3( z - (ValueType) z0 );
						ws = Interp::SCurve3( w - (ValueType) w0 );

						break;


						case QUALITY_BEST:

						xs = Interp::SCurve5( x - (ValueType) x0 );
						ys = Interp::SCurve5( y - (ValueType) y0 );
						zs = Interp::SCurve5( z - (ValueType) z0 );
						ws = Interp::SCurve5( w - (ValueType) w0 );

						break;
					}

					// Now calculate the noise values at each vertex of the cube.  To generate
					// the coherent-noise value at the input point, interpolate these eight
					// noise values using the S-curve value as the interpolant (trilinear
					// interpolation.)
					ValueType n0, n1, ix0, ix1, iy0, iy1, iz0, iz1;
					
					n0   = GradientNoise( x, y, z, w, x0, y0, z0, w0, seed );
					n1   = GradientNoise( x, y, z, w, x1, y0, z0, w0, seed );
					ix0  = Interp::LinearInterp( n0, n1, xs );
					n0   = GradientNoise( x, y, z, w, x0, y1, z0, w0, seed );
					n1   = GradientNoise( x, y, z, w, x1, y1, z0, w0, seed );
					ix1  = Interp::LinearInterp( n0, n1, xs );
					iy0  = Interp::LinearInterp( ix0, ix1, ys );
					n0   = GradientNoise( x, y, z, w, x0, y0, z1, w0, seed );
					n1   = GradientNoise( x, y, z, w, x1, y0, z1, w0, seed );
					ix0  = Interp::LinearInterp( n0, n1, xs );
					n0   = GradientNoise( x, y, z, w, x0, y1, z1, w0, seed );
					n1   = GradientNoise( x, y, z, w, x1, y1, z1, w0, seed );
					ix1  = Interp::LinearInterp( n0, n1, xs );
					iy1  = Interp::LinearInterp( ix0, ix1, ys );
					iz0	 = Interp::LinearInterp( iy0, iy1, zs );

					n0   = GradientNoise( x, y, z, w, x0, y0, z0, w1, seed );
					n1   = GradientNoise( x, y, z, w, x1, y0, z0, w1, seed );
					ix0  = Interp::LinearInterp( n0, n1, xs );
					n0   = GradientNoise( x, y, z, w, x0, y1, z0, w1, seed );
					n1   = GradientNoise( x, y, z, w, x1, y1, z0, w1, seed );
					ix1  = Interp::LinearInterp( n0, n1, xs );
					iy0  = Interp::LinearInterp( ix0, ix1, ys );
					n0   = GradientNoise( x, y, z, w, x0, y0, z1, w1, seed );
					n1   = GradientNoise( x, y, z, w, x1, y0, z1, w1, seed );
					ix0  = Interp::LinearInterp( n0, n1, xs );
					n0   = GradientNoise( x, y, z, w, x0, y1, z1, w1, seed );
					n1   = GradientNoise( x, y, z, w, x1, y1, z1, w1, seed );
					ix1  = Interp::LinearInterp( n0, n1, xs );
					iy1  = Interp::LinearInterp( ix0, ix1, ys );
					iz1	 = Interp::LinearInterp( iy0, iy1, zs );

					return Interp::LinearInterp( iz0, iz1, ws );
				}
				
				static inline
				ValueType
				GradientNoise( ValueType fx, ValueType fy, ValueType fz, ValueType fw, int ix, int iy, int iz, int iw, int seed )
				{
					// Randomly generate a gradient vector given the integer coordinates of the
					// input value.  This implementation generates a random number and uses it
					// as an index into a normalized-vector lookup table.
					int vectorIndex = (
					  X_NOISE_GEN    * ix
					+ Y_NOISE_GEN    * iy
					+ Z_NOISE_GEN    * iz
					+ W_NOISE_GEN	 * iw
					+ SEED_NOISE_GEN * seed)
					& 0xffffffff;
					
					vectorIndex ^= (vectorIndex >> SHIFT_NOISE_GEN);
					vectorIndex &= 0xff;
					
					ValueType	xvGradient = Table::values()[ (vectorIndex << 2)     ];
					ValueType	yvGradient = Table::values()[ (vectorIndex << 2) + 1 ];
					ValueType	zvGradient = Table::values()[ (vectorIndex << 2) + 2 ];
					ValueType	wvGradient = Table::values()[ (vectorIndex << 2) + 3 ];

					// Set up us another vector equal to the distance between the two vectors
					// passed to this function.
					ValueType	xvPoint = (fx - (ValueType) ix);
					ValueType	yvPoint = (fy - (ValueType) iy);
					ValueType	zvPoint = (fz - (ValueType) iz);
					ValueType	wvPoint = (fw - (ValueType) iw);

					// Now compute the dot product of the gradient vector with the distance
					// vector.  The resulting value is gradient noise.  Apply a scaling value
					// so that this noise value ranges from -1.0 to 1.0.
					return ((xvGradient * xvPoint)
							+ (yvGradient * yvPoint)
							+ (zvGradient * zvPoint)
							+ (wvGradient * wvPoint)) * ValueType( 2.12 );
				}
								
			};

		}

	}

}