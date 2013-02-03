template<>
class NoiseGen< float, 2 >
{

	public:
	
	typedef float						ValueType;
	static const unsigned int			Dimension = 2;
	typedef Math< ValueType >			M;
	typedef Interpolations< ValueType >	Interp;
	typedef VectorTable< ValueType >	Table;
	
	
	
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
		
		// Vectorize input coordinates
		M::Vector4F		coordV;
		M::vectorize4F( coordV, x, y, ValueType( 0.0 ), ValueType( 0.0 ) );

		// Map the difference between the coordinates of the input value and the
		// coordinates of the cube's outer-lower-left vertex onto an S-curve.
		M::Vector4F		sCoordV;
		M::Vector4F		xy__0;
		
		M::vectorize4F( xy__0, ValueType( x0 ), ValueType( y0 ), ValueType( 0.0 ), ValueType( 0.0 ) );
		
		// Calculate distance from the "zero" cube coordinates
		M::vectorSub( sCoordV, coordV, xy__0 );
		
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
		
		// Extract interpolated values
		ValueType	xs, ys;
		
		xs = ((float*)&sCoordV)[ 0 ];
		ys = ((float*)&sCoordV)[ 1 ];
		
		// Now calculate the noise values at each vertex of the cube.  To generate
		// the coherent-noise value at the input point, interpolate these eight
		// noise values using the S-curve value as the interpolant (trilinear
		// interpolation.)
		SSE_ALIGN( int			tableIndices[ 4 ] );
		SSE_ALIGN( M::Vector4F	noiseValues[ 1 ] );
		
		
		M::Vector4F		xy01;
		M::Vector4F		__01;
		
		M::vectorize4F( xy01, ValueType( x1 ), ValueType( x0 ), ValueType( y1 ), ValueType( y0 ) );
		M::vectorize4F( __01, ValueType( 0.0 ), ValueType( 0.0 ), ValueType( 0.0 ), ValueType( 0.0 ) );
		
		initializeTableIndices( tableIndices, x0, x1, y0, y1, seed );
		
		calculateNoisePartSSELocal0_4( coordV, xy01, __01, tableIndices, noiseValues[ 0 ] );
		
		M::Vector4F		interpV;
		M::vectorize4F( interpV, xs, xs, xs, xs );
		
		
		ValueType	ix0 = Interp::LinearInterp( ((ValueType*)noiseValues)[ 0 ], ((ValueType*)noiseValues)[ 2 ], xs );
		ValueType	ix1 = Interp::LinearInterp( ((ValueType*)noiseValues)[ 1 ], ((ValueType*)noiseValues)[ 3 ], xs );
		
		return Interp::LinearInterp( ix0, ix1, ys );
	}
	
	static inline
	void
	calculateNoisePartSSELocal0_4( M::Vector4F& coordV, M::Vector4F& xy01, M::Vector4F& __01, int* tableIndices, M::Vector4F& n0 )
	{
		// define local SSE array and vector arrays
		M::Vector4F		noise[ 4 ];
		M::Vector4F*	tableV = (M::Vector4F*) Table::values();
		
		// calculate edge vectors
		noise[ 0 ] = _mm_shuffle_ps( xy01, __01, _MM_SHUFFLE( 3, 1, 3, 1 ) );
		noise[ 1 ] = _mm_shuffle_ps( xy01, __01, _MM_SHUFFLE( 3, 1, 2, 1 ) );
		noise[ 2 ] = _mm_shuffle_ps( xy01, __01, _MM_SHUFFLE( 3, 1, 3, 0 ) );
		noise[ 3 ] = _mm_shuffle_ps( xy01, __01, _MM_SHUFFLE( 3, 1, 2, 0 ) );
		
		// calculate distances
		for( int i = 0; i < 4; ++i )
		{
			M::vectorSub( noise[ i ], coordV, noise[ i ] );
		}
		
		// multiply with gradients
		for( int i = 0; i < 4; ++i )
		{
			M::Vector4F		gradient = _mm_load_ps( (ValueType*)(tableV + tableIndices[ i ]) );
			M::vectorMul( noise[ i ], noise[ i ], gradient );
		}
		
		// horizontal add for dot product
		for( int i = 0; i < 4; i += 2 )
		{
			noise[ i ] = _mm_hadd_ps( noise[ i ], noise[ i + 1 ] );
		}
		for( int i = 0; i < 4; i += 4 )
		{
			noise[ i ] = _mm_hadd_ps( noise[ i ], noise[ i + 2 ] );
		}
		
		// scale noise values
		M::Vector4F		noiseScale;
		M::vectorize4F( noiseScale, ValueType( 2.12 ), ValueType( 2.12 ), ValueType( 2.12 ), ValueType( 2.12 ) );
		
		M::vectorMul( n0, noise[ 0 ], noiseScale );
	}
	
	static inline
	void
	initializeTableIndices( int* indices, int x0, int x1, int y0, int y1, int seed )
	{
		// set initial indices value to seed noise
		int seedNoise = SEED_NOISE_GEN * seed;
		
		for( int i = 0; i < 4; ++i )
		{
			indices[ i ] = seedNoise;
		}
		
		// prepare coordinate noise
		int noise;
		
		noise = X_NOISE_GEN * x0;
		for( int i = 0; i < 4; i += 4 )
		{
			indices[ i ] += noise;
			indices[ i + 1 ] += noise;
		}
		
		noise = X_NOISE_GEN * x1;
		for( int i = 2; i < 4; i += 4 )
		{
			indices[ i ] += noise;
			indices[ i + 1 ] += noise;
		}
		
		noise = Y_NOISE_GEN * y0;
		for( int i = 0; i < 4; i += 2 )
		{
			indices[ i ] += noise;
		}
		
		noise = Y_NOISE_GEN * y1;
		for( int i = 1; i < 4; i += 2 )
		{
			indices[ i ] += noise;
		}
		
		for( int i = 0; i < 4; ++i )
		{
			indices[ i ] ^= (indices[ i ] >> SHIFT_NOISE_GEN);
			indices[ i ] &= 0xff;
		}
	}

	static inline
	ValueType
	SimplexNoise( ValueType xIn, ValueType yIn )
	{
		return ValueType( 0.0 );
	}

};