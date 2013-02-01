template< typename ValueT >
class NoiseGen< ValueT, 4 >
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
	
	
	
	static inline
	ValueType
	SimplexNoise( ValueType xIn, ValueType yIn, ValueType zIn, ValueType wIn )
	{
		ValueType	F4 = 0.309016994; // (Math.sqrt(5.0)-1.0)/4.0
		ValueType	G4 = 0.138196601; // (5.0-Math.sqrt(5.0))/20.0
		
		
		ValueType	s = (xIn + yIn + zIn + wIn) * F4;
		int			i = M::floor( xIn + s );
		int			j = M::floor( yIn + s );
		int			k = M::floor( zIn + s );
		int			l = M::floor( wIn + s );
		ValueType	t = (i + j + k + l) * G4;
		ValueType	X0 = i - t;
		ValueType	Y0 = j - t;
		ValueType	Z0 = k - t;
		ValueType	W0 = l - t;
		ValueType	x0 = xIn - X0;
		ValueType	y0 = yIn - Y0;
		ValueType	z0 = zIn - Z0;
		ValueType	w0 = wIn - W0;
		
		// For the 4D case, the simplex is a 4D shape I won't even try to describe.
		// To find out which of the 24 possible simplices we're in, we need to
		// determine the magnitude ordering of x0, y0, z0 and w0.
		// The method below is a good way of finding the ordering of x,y,z,w and
		// then find the correct traversal order for the simplex we’re in.
		// First, six pair-wise comparisons are performed between each possible pair
		// of the four coordinates, and the results are used to add up binary bits
		// for an integer index.
		int c1 = (x0 > y0) ? 32 : 0;
		int c2 = (x0 > z0) ? 16 : 0;
		int c3 = (y0 > z0) ? 8 : 0;
		int c4 = (x0 > w0) ? 4 : 0;
		int c5 = (y0 > w0) ? 2 : 0;
		int c6 = (z0 > w0) ? 1 : 0;
		int c = c1 + c2 + c3 + c4 + c5 + c6;

		int i1, j1, k1, l1; // The integer offsets for the second simplex corner
		int i2, j2, k2, l2; // The integer offsets for the third simplex corner
		int i3, j3, k3, l3; // The integer offsets for the fourth simplex corner
		
		// A lookup table to traverse the simplex around a given point in 4D.
		// Details can be found where this table is used, in the 4D noise method.
		/* TODO: This should not be required, backport it from Bill's GLSL code! */
		static unsigned char simplex[64][4] =
		{
			{0,1,2,3},{0,1,3,2},{0,0,0,0},{0,2,3,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,2,3,0},
			{0,2,1,3},{0,0,0,0},{0,3,1,2},{0,3,2,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,3,2,0},
			{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
			{1,2,0,3},{0,0,0,0},{1,3,0,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,3,0,1},{2,3,1,0},
			{1,0,2,3},{1,0,3,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,0,3,1},{0,0,0,0},{2,1,3,0},
			{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
			{2,0,1,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,0,1,2},{3,0,2,1},{0,0,0,0},{3,1,2,0},
			{2,1,0,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,1,0,2},{0,0,0,0},{3,2,0,1},{3,2,1,0}
		};

		// simplex[c] is a 4-vector with the numbers 0, 1, 2 and 3 in some order.
		// Many values of c will never occur, since e.g. x>y>z>w makes x<z, y<w and x<w
		// impossible. Only the 24 indices which have non-zero entries make any sense.
		// We use a thresholding to set the coordinates in turn from the largest magnitude.
		// The number 3 in the "simplex" array is at the position of the largest coordinate.
		i1 = simplex[c][0]>=3 ? 1 : 0;
		j1 = simplex[c][1]>=3 ? 1 : 0;
		k1 = simplex[c][2]>=3 ? 1 : 0;
		l1 = simplex[c][3]>=3 ? 1 : 0;
		// The number 2 in the "simplex" array is at the second largest coordinate.
		i2 = simplex[c][0]>=2 ? 1 : 0;
		j2 = simplex[c][1]>=2 ? 1 : 0;
		k2 = simplex[c][2]>=2 ? 1 : 0;
		l2 = simplex[c][3]>=2 ? 1 : 0;
		// The number 1 in the "simplex" array is at the second smallest coordinate.
		i3 = simplex[c][0]>=1 ? 1 : 0;
		j3 = simplex[c][1]>=1 ? 1 : 0;
		k3 = simplex[c][2]>=1 ? 1 : 0;
		l3 = simplex[c][3]>=1 ? 1 : 0;
		// The fifth corner has all coordinate offsets = 1, so no need to look that up.

		ValueType	x1 = x0 - i1 + G4; // Offsets for second corner in (x,y,z,w) coords
		ValueType	y1 = y0 - j1 + G4;
		ValueType	z1 = z0 - k1 + G4;
		ValueType	w1 = w0 - l1 + G4;
		ValueType	x2 = x0 - i2 + 2.0f*G4; // Offsets for third corner in (x,y,z,w) coords
		ValueType	y2 = y0 - j2 + 2.0f*G4;
		ValueType	z2 = z0 - k2 + 2.0f*G4;
		ValueType	w2 = w0 - l2 + 2.0f*G4;
		ValueType	x3 = x0 - i3 + 3.0f*G4; // Offsets for fourth corner in (x,y,z,w) coords
		ValueType	y3 = y0 - j3 + 3.0f*G4;
		ValueType	z3 = z0 - k3 + 3.0f*G4;
		ValueType	w3 = w0 - l3 + 3.0f*G4;
		ValueType	x4 = x0 - 1.0f + 4.0f*G4; // Offsets for last corner in (x,y,z,w) coords
		ValueType	y4 = y0 - 1.0f + 4.0f*G4;
		ValueType	z4 = z0 - 1.0f + 4.0f*G4;
		ValueType	w4 = w0 - 1.0f + 4.0f*G4;

		// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
		unsigned char*	perm = PermTable::values();
		int ii = i & 0xff;
		int jj = j & 0xff;
		int kk = k & 0xff;
		int ll = l & 0xff;
		
		// Calculate the contributions form corners
		ValueType	n0, n1, n2, n3, n4;
		
		ValueType	t0 = 0.6f - x0*x0 - y0*y0 - z0*z0 - w0*w0;
		if( t0 < 0.0f )
		{
			n0 = 0.0f;
		}
		else
		{
			t0 *= t0;
			n0 = t0 * t0 * grad( perm[ ii+perm[ jj+perm[ kk+perm[ ll ] ] ] ], x0, y0, z0, w0 );
		}

		ValueType	t1 = 0.6f - x1*x1 - y1*y1 - z1*z1 - w1*w1;
		if( t1 < 0.0f )
		{
			n1 = 0.0f;
		}
		else
		{
			t1 *= t1;
			n1 = t1 * t1 * grad( perm[ ii+i1+perm[ jj+j1+perm[ kk+k1+perm[ ll+l1 ] ] ] ], x1, y1, z1, w1 );
		}

		ValueType	t2 = 0.6f - x2*x2 - y2*y2 - z2*z2 - w2*w2;
		if( t2 < 0.0f )
		{
			n2 = 0.0f;
		}
		else
		{
			t2 *= t2;
			n2 = t2 * t2 * grad( perm[ ii+i2+perm[ jj+j2+perm[ kk+k2+perm[ ll+l2 ] ] ] ], x2, y2, z2, w2 );
		}

		ValueType	t3 = 0.6f - x3*x3 - y3*y3 - z3*z3 - w3*w3;
		if( t3 < 0.0f )
		{
			n3 = 0.0f;
		}
		else
		{
			t3 *= t3;
			n3 = t3 * t3 * grad( perm[ ii+i3+perm[ jj+j3+perm[ kk+k3+perm[ ll+l3 ] ] ] ], x3, y3, z3, w3 );
		}

		ValueType	t4 = 0.6f - x4*x4 - y4*y4 - z4*z4 - w4*w4;
		if( t4 < 0.0f )
		{
			n4 = 0.0f;
		}
		else
		{
			t4 *= t4;
			n4 = t4 * t4 * grad( perm[ ii+1+perm[ jj+1+perm[ kk+1+perm[ ll+1 ] ] ] ], x4, y4, z4, w4 );
		}
		
		// Add contributions from each corner to get the final noise value.
		// The result is scaled to return values in the interval [-1,1].
		return 27.0f * (n0 + n1 + n2 + n3 + n4);
	}
	
	static inline
	ValueType
	grad( int hash, ValueType x, ValueType y, ValueType z, ValueType t )
	{
		int h = hash & 31;      // Convert low 5 bits of hash code into 32 simple
		float u = h<24 ? x : y; // gradient directions, and compute dot product.
		float v = h<16 ? y : z;
		float w = h<8 ? z : t;
		return ((h&1)? -u : u) + ((h&2)? -v : v) + ((h&4)? -w : w);
	}
	
};