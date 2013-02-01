template< typename ValueT >
class NoiseGen< ValueT, 1 >
{

	public:
	
	typedef ValueT						ValueType;
	static const unsigned int			Dimension = 1;
	typedef Math< ValueType >			M;
	typedef Interpolations< ValueType >	Interp;
	typedef VectorTable< ValueType >	Table;
	
	
	
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