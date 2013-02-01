#include <cmath>




template< typename ValueT >
class NoiseGen< ValueT, 2 >
{

	public:
	
	typedef ValueT								ValueType;
	static const unsigned int					Dimension = 2;
	typedef Math< ValueType >					M;
	typedef Interpolations< ValueType >			Interp;
	typedef VectorTable< ValueType >			Table;
	typedef PermutationTable< unsigned char >	PermTable;



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


	static inline
	ValueType
	SimplexNoise( ValueType xIn, ValueType yIn )
	{
		ValueType	F2 = 0.5 * (sqrt( 3.0 ) - 1.0 );
		ValueType	G2 = (3.0-  sqrt( 3.0 )) / 6.0;
		
		
		// Skew the input space to determine which simplex cell we're in
		ValueType	s = (xIn + yIn) * F2; // Hairy factor for 2D
		int			i = M::floor( xIn + s );
		int			j = M::floor( yIn + s );
		ValueType	t = (i + j) * G2;
		ValueType	X0 = i - t; // Unskew the cell origin back to (x,y) space
		ValueType	Y0 = j - t;
		ValueType	x0 = xIn - X0; // The x,y distances from the cell origin
		ValueType	y0 = yIn - Y0;
		
		// For the 2D case, the simplex shape is an equilateral triangle.
		// Determine which simplex we are in.
		int		i1;
		int		j1; // Offsets for second (middle) corner of simplex in (i,j) coords
		
		// lower triangle, XY order: (0,0)->(1,0)->(1,1)
		if( x0 > y0 )
		{
			i1 = 1;
			j1=0;
		}
		// upper triangle, YX order: (0,0)->(0,1)->(1,1)
		else
		{
			i1=0;
			j1=1;
		}
		
		// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
		// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
		// c = (3-sqrt(3))/6
		ValueType	x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
		ValueType	y1 = y0 - j1 + G2;
		ValueType	x2 = x0 - 1.0 + 2.0 * G2; // Offsets for last corner in (x,y) unskewed coords
		ValueType	y2 = y0 - 1.0 + 2.0 * G2;
		
		// Work out the hashed gradient indices of the three simplex corners
		unsigned char*	perm = PermTable::values();
		int ii = i & 255;
		int jj = j & 255;
		
		// Calculate the contribution from the three corners
		ValueType	n0, n1, n2; // Noise contributions from the three corners
		
		ValueType	t0 = 0.5 - (x0 * x0) - (y0 * y0);
		if( t0 < 0 )
		{
			n0 = 0.0;
		}
		else
		{
			t0 *= t0;
			n0 = t0 * t0 * grad( perm[ ii + perm[ jj ] ], x0, y0 );
		}
		
		ValueType	t1 = 0.5 - (x1 * x1) - (y1 * y1);
		if( t1 < 0 )
		{
			n1 = 0.0;
		}
		else
		{
			t1 *= t1;
			n1 = t1 * t1 * grad( perm[ ii + i1 + perm[ jj + j1 ] ], x1, y1 );
		}
		
		ValueType	t2 = 0.5 - (x2 * x2) - (y2 * y2);
		if( t2 < 0 )
		{
			n2 = 0.0;
		}
		else
		{
			t2 *= t2;
			n2 = t2 * t2 * grad( perm[ ii + 1 + perm[ jj + 1 ] ], x2, y2 );
		}
		
		// Add contributions from each corner to get the final noise value.
		// The result is scaled to return values in the interval [-1,1].
		return 40.0 * (n0 + n1 + n2);
	}
	
	static inline
	ValueType
	grad( int hash, ValueType x, ValueType y )
	{
		int			h = hash & 7;      // Convert low 3 bits of hash code
		ValueType	u = h < 4 ? x : y;  // into 8 simple gradient directions,
		ValueType	v = h < 4 ? y : x;  // and compute the dot product with (x,y).
		return ((h & 1) ? -u : u) + ((h & 2) ? (-2.0f * v) : (2.0f * v));
	}
	
};