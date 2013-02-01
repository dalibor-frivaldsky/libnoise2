template<>
class NoiseGen< float, 4 >
{

	public:
	
	typedef float						ValueType;
	static const unsigned int			Dimension = 4;
	typedef Math< ValueType >			M;
	typedef Interpolations< ValueType >	Interp;
	typedef VectorTable< ValueType >	Table;



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
		
		// Vectorize input coordinates
		M::Vector4F		coordV;
		M::vectorize4F( coordV, x, y, z, w );

		// Map the difference between the coordinates of the input value and the
		// coordinates of the cube's outer-lower-left vertex onto an S-curve.
		M::Vector4F		sCoordV;
		M::Vector4F		xyzw0;
		
		M::vectorize4F( xyzw0, ValueType( x0 ), ValueType( y0 ), ValueType( z0 ), ValueType( w0 ) );
		
		// Calculate distance from the "zero" cube coordinates
		M::vectorSub( sCoordV, coordV, xyzw0 );
		
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
		ValueType	xs, ys, zs, ws;
		
		xs = ((float*)&sCoordV)[ 0 ];
		ys = ((float*)&sCoordV)[ 1 ];
		zs = ((float*)&sCoordV)[ 2 ];
		ws = ((float*)&sCoordV)[ 3 ];

		// Now calculate the noise values at each vertex of the cube.  To generate
		// the coherent-noise value at the input point, interpolate these eight
		// noise values using the S-curve value as the interpolant (trilinear
		// interpolation.)
		
		SSE_ALIGN( int			tableIndices[ 16 ] );
		SSE_ALIGN( M::Vector4F	noiseValues[ 4 ] );
		
		
		M::Vector4F		xy01;
		M::Vector4F		zw01;
		M::Vector4I		xyi01;
		M::Vector4I		zwi01;
		M::Vector4I		seedV;
		
		
		M::vectorize4F( xy01, ValueType( x1 ), ValueType( x0 ), ValueType( y1 ), ValueType( y0 ) );
		M::vectorize4F( zw01, ValueType( z1 ), ValueType( z0 ), ValueType( w1 ), ValueType( w0 ) );
		M::vectorize4I( xyi01, X_NOISE_GEN * x0, X_NOISE_GEN * x1, Y_NOISE_GEN * y0, Y_NOISE_GEN * y1 );
		M::vectorize4I( zwi01, Z_NOISE_GEN * z0, Z_NOISE_GEN * z1, W_NOISE_GEN * w0, W_NOISE_GEN * w1 );
		M::vectorize4I( seedV, SEED_NOISE_GEN * seed, SEED_NOISE_GEN * seed, SEED_NOISE_GEN * seed, SEED_NOISE_GEN * seed );
		
		initializeTableIndices( tableIndices, x0, x1, y0, y1, z0, z1, w0, w1, seed );
		
		calculateNoisePartSSELocal0_7( coordV, xy01, zw01, tableIndices, noiseValues[ 0 ], noiseValues[ 1 ] );
		calculateNoisePartSSELocal8_15( coordV, xy01, zw01, tableIndices + 8, noiseValues[ 2 ], noiseValues[ 3 ] );
		
		M::Vector4F		interpV;
		M::vectorize4F( interpV, xs, xs, xs, xs );
		
		noiseValues[ 0 ] = Interp::LinearInterpV( noiseValues[ 0 ], noiseValues[ 2 ], interpV );
		noiseValues[ 1 ] = Interp::LinearInterpV( noiseValues[ 1 ], noiseValues[ 3 ], interpV );
		
		M::vectorize4F( interpV, ys, ys, ys, ys );
		
		noiseValues[ 0 ] = Interp::LinearInterpV( noiseValues[ 0 ], noiseValues[ 1 ], interpV );
		
		ValueType	iz0 = Interp::LinearInterp( ((ValueType*)noiseValues)[ 0 ], ((ValueType*)noiseValues)[ 2 ], zs );
		ValueType	iz1 = Interp::LinearInterp( ((ValueType*)noiseValues)[ 1 ], ((ValueType*)noiseValues)[ 3 ], zs );
		
		return Interp::LinearInterp( iz0, iz1, ws );
	}
	
	static inline
	void
	calculateNoisePartSSELocal0_7( M::Vector4F& coordV, M::Vector4F& xy01, M::Vector4F& zw01, int* tableIndices, M::Vector4F& n0, M::Vector4F& n1 )
	{
		// define local SSE array and vector arrays
		M::Vector4F		noise[ 8 ];
		M::Vector4F*	tableV = (M::Vector4F*) Table::values();
		
		// calculate edge vectors
		//static unsigned int		baseMask = (3<<6) | (1<<4) | (3<<2) | 1;
		
		#define SHUFFLE_MASK( i, baseMask )		(baseMask) ^ \
												( ( ((i)|8) << 6 ) | ( ((i)|4) << 4 ) | ( ((i)|2) << 2 ) | ((i)|1) )
		
		noise[ 0 ] = _mm_shuffle_ps( xy01, zw01, _MM_SHUFFLE( 3, 1, 3, 1 ) );
		noise[ 1 ] = _mm_shuffle_ps( xy01, zw01, _MM_SHUFFLE( 2, 1, 3, 1 ) );
		noise[ 2 ] = _mm_shuffle_ps( xy01, zw01, _MM_SHUFFLE( 3, 0, 3, 1 ) );
		noise[ 3 ] = _mm_shuffle_ps( xy01, zw01, _MM_SHUFFLE( 2, 0, 3, 1 ) );
		noise[ 4 ] = _mm_shuffle_ps( xy01, zw01, _MM_SHUFFLE( 3, 1, 2, 1 ) );
		noise[ 5 ] = _mm_shuffle_ps( xy01, zw01, _MM_SHUFFLE( 2, 1, 2, 1 ) );
		noise[ 6 ] = _mm_shuffle_ps( xy01, zw01, _MM_SHUFFLE( 3, 0, 2, 1 ) );
		noise[ 7 ] = _mm_shuffle_ps( xy01, zw01, _MM_SHUFFLE( 2, 0, 2, 1 ) );
		
		// calculate distances
		for( int i = 0; i < 8; ++i )
		{
			M::vectorSub( noise[ i ], coordV, noise[ i ] );
		}
		
		// multiply with gradients
		for( int i = 0; i < 8; ++i )
		{
			M::Vector4F		gradient = _mm_load_ps( (ValueType*)(tableV + tableIndices[ i ]) );
			M::vectorMul( noise[ i ], noise[ i ], gradient );
		}
		
		// horizontal add for dot product
		for( int i = 0; i < 8; i += 2 )
		{
			noise[ i ] = _mm_hadd_ps( noise[ i ], noise[ i + 1 ] );
		}
		for( int i = 0; i < 8; i += 4 )
		{
			noise[ i ] = _mm_hadd_ps( noise[ i ], noise[ i + 2 ] );
		}
		
		// scale noise values
		M::Vector4F		noiseScale;
		M::vectorize4F( noiseScale, ValueType( 2.12 ), ValueType( 2.12 ), ValueType( 2.12 ), ValueType( 2.12 ) );
		
		M::vectorMul( n0, noise[ 0 ], noiseScale );
		M::vectorMul( n1, noise[ 4 ], noiseScale );
	}
	
	static inline
	void
	calculateNoisePartSSELocal8_15( M::Vector4F& coordV, M::Vector4F& xy01, M::Vector4F& zw01, int* tableIndices, M::Vector4F& n0, M::Vector4F& n1 )
	{
		// define local SSE array and vector arrays
		M::Vector4F		noise[ 8 ];
		M::Vector4F*	tableV = (M::Vector4F*) Table::values();
		
		// calculate edge vectors
		noise[ 0 ] = _mm_shuffle_ps( xy01, zw01, _MM_SHUFFLE( 3, 1, 3, 0 ) );
		noise[ 1 ] = _mm_shuffle_ps( xy01, zw01, _MM_SHUFFLE( 2, 1, 3, 0 ) );
		noise[ 2 ] = _mm_shuffle_ps( xy01, zw01, _MM_SHUFFLE( 3, 0, 3, 0 ) );
		noise[ 3 ] = _mm_shuffle_ps( xy01, zw01, _MM_SHUFFLE( 2, 0, 3, 0 ) );
		noise[ 4 ] = _mm_shuffle_ps( xy01, zw01, _MM_SHUFFLE( 3, 1, 2, 0 ) );
		noise[ 5 ] = _mm_shuffle_ps( xy01, zw01, _MM_SHUFFLE( 2, 1, 2, 0 ) );
		noise[ 6 ] = _mm_shuffle_ps( xy01, zw01, _MM_SHUFFLE( 3, 0, 2, 0 ) );
		noise[ 7 ] = _mm_shuffle_ps( xy01, zw01, _MM_SHUFFLE( 2, 0, 2, 0 ) );
		
		// calculate distances
		for( int i = 0; i < 8; ++i )
		{
			M::vectorSub( noise[ i ], coordV, noise[ i ] );
		}
		
		// multiply with gradients
		for( int i = 0; i < 8; ++i )
		{
			M::Vector4F		gradient = _mm_load_ps( (ValueType*)(tableV + tableIndices[ i ]) );
			M::vectorMul( noise[ i ], noise[ i ], gradient );
		}
		
		// horizontal add for dot product
		for( int i = 0; i < 8; i += 2 )
		{
			noise[ i ] = _mm_hadd_ps( noise[ i ], noise[ i + 1 ] );
		}
		for( int i = 0; i < 8; i += 4 )
		{
			noise[ i ] = _mm_hadd_ps( noise[ i ], noise[ i + 2 ] );
		}
		
		// scale and store noise values
		M::Vector4F		noiseScale;
		M::vectorize4F( noiseScale, ValueType( 2.12 ), ValueType( 2.12 ), ValueType( 2.12 ), ValueType( 2.12 ) );
		
		M::vectorMul( n0, noise[ 0 ], noiseScale );
		M::vectorMul( n1, noise[ 4 ], noiseScale );
	}
	
	static inline
	void
	initializeTableIndices( int* indices, int x0, int x1, int y0, int y1, int z0, int z1, int w0, int w1, int seed )
	{
		//M::Vector4I*	indicesV = (M::Vector4I*) indices;
		
		// set initial indices value to seed noise
		int seedNoise = SEED_NOISE_GEN * seed;
		//M::Vector4I		seedNoiseV;
		//M::vectorize4I( seedNoiseV, seedNoise, seedNoise, seedNoise, seedNoise );
		
		for( int i = 0; i < 16; ++i )
		//for( int i = 0; i < 4; ++i )
		{
			//_mm_store_si128( indicesV + i, seedNoiseV );
			indices[ i ] = seedNoise;
		}
		
		// prepare coordinate noise
		int noise;
		
		noise = X_NOISE_GEN * x0;
		for( int i = 0; i < 8; ++i )
		{
			indices[ i ] += noise;
		}
		
		noise = X_NOISE_GEN * x1;
		for( int i = 8; i < 16; ++i )
		{
			indices[ i ] += noise;
		}
		
		noise = Y_NOISE_GEN * y0;
		for( int i = 0; i < 16; i += 8 )
		{
			indices[ i ] += noise;
			indices[ i + 1 ] += noise;
			indices[ i + 2 ] += noise;
			indices[ i + 3 ] += noise;
		}
		
		noise = Y_NOISE_GEN * y1;
		for( int i = 4; i < 16; i += 8 )
		{
			indices[ i ] += noise;
			indices[ i + 1 ] += noise;
			indices[ i + 2 ] += noise;
			indices[ i + 3 ] += noise;
		}
		
		noise = Z_NOISE_GEN * z0;
		for( int i = 0; i < 16; i += 4 )
		{
			indices[ i ] += noise;
			indices[ i + 1 ] += noise;
		}
		
		noise = Z_NOISE_GEN * z1;
		for( int i = 2; i < 16; i += 4 )
		{
			indices[ i ] += noise;
			indices[ i + 1 ] += noise;
		}
		
		noise = W_NOISE_GEN * w0;
		for( int i = 0; i < 16; i += 2 )
		{
			indices[ i ] += noise;
		}
		
		noise = W_NOISE_GEN * w1;
		for( int i = 1; i < 16; i += 2 )
		{
			indices[ i ] += noise;
		}
		
		for( int i = 0; i < 16; ++i )
		{
			indices[ i ] ^= (indices[ i ] >> SHIFT_NOISE_GEN);
			indices[ i ] &= 0xff;
		}
	}

};