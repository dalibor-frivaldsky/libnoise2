template< typename ValueT >
class NoiseGen< ValueT, 3 >
{

	public:
	
	typedef ValueT								ValueType;
	static const unsigned int					Dimension = 4;
	typedef Math< ValueType >					M;
	typedef Interpolations< ValueType >			Interp;
	typedef VectorTable< ValueType >			Table;
	typedef PermutationTable< unsigned char >	PermTable;



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
	
	
	
	static inline
	ValueType
	SimplexNoise( ValueType xIn, ValueType yIn, ValueType zIn )
	{
		ValueType	F3 = 0.333333333;
		ValueType	G3 = 0.166666667;
		
		
		ValueType	s = (xIn + yIn + zIn) * F3;
		int			i = M::floor( xIn + s );
		int			j = M::floor( yIn + s );
		int			k = M::floor( zIn + s );
		ValueType	t = (i + j + k) * G3;
		ValueType	X0 = i - t;
		ValueType	Y0 = j - t;
		ValueType	Z0 = k - t;
		ValueType	x0 = xIn - X0;
		ValueType	y0 = yIn - Y0;
		ValueType	z0 = zIn - Z0;
		
		// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
		// Determine which simplex we are in.
		int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
		int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords

		/* This code would benefit from a backport from the GLSL version! */
		if( x0 >= y0 )
		{
			if( y0 >= z0 )
				{i1=1; j1=0; k1=0; i2=1; j2=1; k2=0; } // X Y Z order
			else if( x0 >= z0 )
				{ i1=1; j1=0; k1=0; i2=1; j2=0; k2=1; } // X Z Y order
			else
				{ i1=0; j1=0; k1=1; i2=1; j2=0; k2=1; } // Z X Y order
		}
		else // x0<y0
		{
			if( y0 <z0 )
				{ i1=0; j1=0; k1=1; i2=0; j2=1; k2=1; } // Z Y X order
			else if( x0 < z0 )
				{ i1=0; j1=1; k1=0; i2=0; j2=1; k2=1; } // Y Z X order
			else
				{ i1=0; j1=1; k1=0; i2=1; j2=1; k2=0; } // Y X Z order
		}

		// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
		// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
		// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
		// c = 1/6.

		ValueType	x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
		ValueType	y1 = y0 - j1 + G3;
		ValueType	z1 = z0 - k1 + G3;
		ValueType	x2 = x0 - i2 + 2.0f * G3; // Offsets for third corner in (x,y,z) coords
		ValueType	y2 = y0 - j2 + 2.0f * G3;
		ValueType	z2 = z0 - k2 + 2.0f * G3;
		ValueType	x3 = x0 - 1.0f + 3.0f * G3; // Offsets for last corner in (x,y,z) coords
		ValueType	y3 = y0 - 1.0f + 3.0f * G3;
		ValueType	z3 = z0 - 1.0f + 3.0f * G3;

		// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
		unsigned char*	perm = PermTable::values();
		int ii = i & 0xff;
		int jj = j & 0xff;
		int kk = k & 0xff;
		
		// Calculate the contributions form corners
		ValueType	n0, n1, n2, n3;
		
		ValueType	t0 = 0.6 - x0 * x0 - y0 * y0 - z0 * z0;
		if( t0 < 0.0 )
		{
			n0 = 0.0;
		}
		else
		{
			t0 *= t0;
			n0 = t0 * t0 * grad( perm[ ii+perm[ jj+perm[ kk ] ] ], x0, y0, z0 );
		}

		ValueType	t1 = 0.6 - x1 * x1 - y1 * y1 - z1 * z1;
		if( t1 < 0.0 )
		{
			n1 = 0.0;
		}
		else
		{
			t1 *= t1;
			n1 = t1 * t1 * grad( perm[ ii+i1 + perm[ jj+j1+perm[ kk+k1 ] ] ], x1, y1, z1 );
		}

		ValueType	t2 = 0.6 - x2*x2 - y2*y2 - z2*z2;
		if( t2 < 0.0 )
		{
			n2 = 0.0;
		}
		else
		{
			t2 *= t2;
			n2 = t2 * t2 * grad( perm[ ii+i2+perm[ jj+j2+perm[ kk+k2 ] ] ], x2, y2, z2 );
		}

		ValueType	t3 = 0.6 - x3*x3 - y3*y3 - z3*z3;
		if( t3 < 0.0 )
		{
			n3 = 0.0;
		}
		else
		{
			t3 *= t3;
			n3 = t3 * t3 * grad( perm[ ii+1+perm[ jj+1+perm[ kk+1 ] ] ], x3, y3, z3 );
		}
		
		// Add contributions from each corner to get the final noise value.
		// The result is scaled to return values in the interval [-1,1].
		return 32.0 * (n0 + n1 + n2 + n3);
	}
	
	static inline
	ValueType
	grad( int hash, ValueType x, ValueType y, ValueType z )
	{
		int			h = hash & 15;     // Convert low 4 bits of hash code into 12 simple
		ValueType	u = h<8 ? x : y; // gradient directions, and compute dot product.
		ValueType	v = h<4 ? y : h==12 || h==14 ? x : z; // Fix repeats at h = 12 to 15
		return ((h&1)? -u : u) + ((h&2)? -v : v);
	}

	static inline
	int
	IntValueNoise( int x, int y, int z, int seed = 0 )
	{
		// All constants are primes and must remain prime in order for this noise
		// function to work correctly.
		int n = (
		  X_NOISE_GEN    * x
		+ Y_NOISE_GEN    * y
		+ Z_NOISE_GEN    * z
		+ SEED_NOISE_GEN * seed)
		& 0x7fffffff;
		n = (n >> 13) ^ n;
		return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
	}

	static inline
	ValueType
	ValueNoise( int x, int y, int z, int seed = 0 )
	{
		return 1.0 - ((double)IntValueNoise( x, y, z, seed ) / 1073741824.0);
	}
	
};