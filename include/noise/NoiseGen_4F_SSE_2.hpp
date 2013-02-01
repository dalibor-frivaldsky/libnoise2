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
	GradientCoherentNoise( M::Vector4F& coordV, int seed = 0, NoiseQuality noiseQuality = QUALITY_STD )
	{
		// Create a unit-length cube aligned along an integer boundary.  This cube
		// surrounds the input point.
		M::Vector4I		onesV = M::vectorizeOne( 1 );
		M::Vector4I		coord0V = M::floatToIntTruncated( coordV );
		M::Vector4I		coordSigns = M::shiftRightLogical( coordV, 31 );
		coord0V = M::subtract( coord0V, coordSigns );
		M::Vector4I		coord1V = M::add( coord0V, onesV );

		SSE_ALIGN( int	cubeBoundariesVector[ 4 ] );
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
		M::Vector4F		sCoordV;
		M::Vector4F		xyzw0 = M::intToFloat( coord0V );
		M::Vector4F		xyzw1 = M::intToFloat( coord1V );
		
		// Calculate distance from the "zero" cube coordinates
		sCoordV = M::subtract( coordV, xyzw0 );
		
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
		
		M::Vector4F		x0y0x1y1 = _mm_shuffle_ps( xyzw0, xyzw1, _MM_SHUFFLE( 0, 1, 0, 1 ) );
		M::Vector4F		z0w0z1w1 = _mm_shuffle_ps( xyzw0, xyzw1, _MM_SHUFFLE( 2, 3, 2, 3 ) );
		
		int				indexBase;
		M::Vector4F		interpV;
		M::Vector4F		n1;
		M::Vector4F		n2;

		indexBase = SEED_NOISE_GEN * seed + X_NOISE_GEN * x0 + Y_NOISE_GEN * y0;
		calculateNoisePart< Shuffle_0_3 >( coordV, x0y0x1y1, z0w0z1w1, indexBase, z0, z1, w0, w1, n1 );

		indexBase = SEED_NOISE_GEN * seed + X_NOISE_GEN * x1 + Y_NOISE_GEN * y0;
		calculateNoisePart< Shuffle_8_11 >( coordV, x0y0x1y1, z0w0z1w1, indexBase, z0, z1, w0, w1, n2 );

		M::vectorDistribute1st( interpV, sCoordV );
		M::Vector4F		nx1 = Interp::LinearInterpV( n1, n2, interpV );


		indexBase = SEED_NOISE_GEN * seed + X_NOISE_GEN * x0 + Y_NOISE_GEN * y1;
		calculateNoisePart< Shuffle_4_7 >( coordV, x0y0x1y1, z0w0z1w1, indexBase, z0, z1, w0, w1, n1 );

		indexBase = SEED_NOISE_GEN * seed + X_NOISE_GEN * x1 + Y_NOISE_GEN * y1;
		calculateNoisePart< Shuffle_12_15 >( coordV, x0y0x1y1, z0w0z1w1, indexBase, z0, z1, w0, w1, n2 );

		M::Vector4F		nx2 = Interp::LinearInterpV( n1, n2, interpV );

		M::vectorDistribute2nd( interpV, sCoordV );
		M::Vector4F		ny = Interp::LinearInterpV( nx1, nx2, interpV );
		M::Vector4F		ny1 = _mm_shuffle_ps( ny, ny, _MM_SHUFFLE( 1, 1, 0, 0 ) );
		M::Vector4F		ny2 = _mm_shuffle_ps( ny, ny, _MM_SHUFFLE( 3, 3, 2, 2 ) );

		M::vectorDistribute3rd( interpV, sCoordV );
		M::Vector4F		nz = Interp::LinearInterpV( ny1, ny2, interpV );
		M::Vector4F		nz1 = _mm_shuffle_ps( nz, nz, _MM_SHUFFLE( 0, 0, 0, 0 ) );
		M::Vector4F		nz2 = _mm_shuffle_ps( nz, nz, _MM_SHUFFLE( 2, 2, 2, 2 ) );

		M::vectorDistribute4th( interpV, sCoordV );
		M::Vector4F		nw = Interp::LinearInterpV( nz1, nz2, interpV );

		return ((float*)&nw)[ 0 ];
	}



private:

	struct Shuffle_0_3
	{
		static const int		Shuffle_00 = _MM_SHUFFLE( 0, 1, 0, 1 );
		static const int		Shuffle_01 = _MM_SHUFFLE( 2, 1, 0, 1 );
		static const int		Shuffle_10 = _MM_SHUFFLE( 0, 3, 0, 1 );
		static const int		Shuffle_11 = _MM_SHUFFLE( 2, 3, 0, 1 );
	};

	struct Shuffle_4_7
	{
		static const int		Shuffle_00 = _MM_SHUFFLE( 0, 1, 2, 1 );
		static const int		Shuffle_01 = _MM_SHUFFLE( 2, 1, 2, 1 );
		static const int		Shuffle_10 = _MM_SHUFFLE( 0, 3, 2, 1 );
		static const int		Shuffle_11 = _MM_SHUFFLE( 2, 3, 2, 1 );
	};

	struct Shuffle_8_11
	{
		static const int		Shuffle_00 = _MM_SHUFFLE( 0, 1, 0, 3 );
		static const int		Shuffle_01 = _MM_SHUFFLE( 2, 1, 0, 3 );
		static const int		Shuffle_10 = _MM_SHUFFLE( 0, 3, 0, 3 );
		static const int		Shuffle_11 = _MM_SHUFFLE( 2, 3, 0, 3 );
	};

	struct Shuffle_12_15
	{
		static const int		Shuffle_00 = _MM_SHUFFLE( 0, 1, 2, 3 );
		static const int		Shuffle_01 = _MM_SHUFFLE( 2, 1, 2, 3 );
		static const int		Shuffle_10 = _MM_SHUFFLE( 0, 3, 2, 3 );
		static const int		Shuffle_11 = _MM_SHUFFLE( 2, 3, 2, 3 );
	};



	template< typename ShufflePolicy >
	static inline
	void
	calculateNoisePart( M::Vector4F& coordV, M::Vector4F& x0y0x1y1, M::Vector4F& z0w0z1w1,
						int indexBase, int iz0, int iz1, int iw0, int iw1,
						M::Vector4F& n0 )
	{
		// define local SSE array and vector arrays
		ValueType*		tableV = Table::values();
		
		// multiply with gradients
		int				tableIndex;
		M::Vector4F		gradient;

		M::Vector4F		n1 = _mm_shuffle_ps( x0y0x1y1, z0w0z1w1, ShufflePolicy::Shuffle_00 );
		n1 = M::subtract( coordV, n1 );
		tableIndex = indexBase + Z_NOISE_GEN * iz0 + W_NOISE_GEN * iw0;
		tableIndex ^= (tableIndex >> SHIFT_NOISE_GEN);
		tableIndex &= 0xff;
		gradient = _mm_load_ps( (ValueType*)(tableV + tableIndex * 4) );
		M::vectorMul( n1, n1, gradient );

		M::Vector4F		n2 = _mm_shuffle_ps( x0y0x1y1, z0w0z1w1, ShufflePolicy::Shuffle_01 );
		n2 = M::subtract( coordV, n2 );
		tableIndex = indexBase + Z_NOISE_GEN * iz0 + W_NOISE_GEN * iw1;
		tableIndex ^= (tableIndex >> SHIFT_NOISE_GEN);
		tableIndex &= 0xff;
		gradient = _mm_load_ps( (ValueType*)(tableV + tableIndex * 4) );
		M::vectorMul( n2, n2, gradient );

		M::Vector4F		nd1 = _mm_hadd_ps( n1, n2 );

		n1 = _mm_shuffle_ps( x0y0x1y1, z0w0z1w1, ShufflePolicy::Shuffle_10 );
		n1 = M::subtract( coordV, n1 );
		tableIndex = indexBase + Z_NOISE_GEN * iz1 + W_NOISE_GEN * iw0;
		tableIndex ^= (tableIndex >> SHIFT_NOISE_GEN);
		tableIndex &= 0xff;
		gradient = _mm_load_ps( (ValueType*)(tableV + tableIndex * 4) );
		M::vectorMul( n1, n1, gradient );

		n2 = _mm_shuffle_ps( x0y0x1y1, z0w0z1w1, ShufflePolicy::Shuffle_11 );
		n2 = M::subtract( coordV, n2 );
		tableIndex = indexBase + Z_NOISE_GEN * iz1 + W_NOISE_GEN * iw1;
		tableIndex ^= (tableIndex >> SHIFT_NOISE_GEN);
		tableIndex &= 0xff;
		gradient = _mm_load_ps( (ValueType*)(tableV + tableIndex * 4) );
		M::vectorMul( n2, n2, gradient );

		M::Vector4F		nd2 = _mm_hadd_ps( n1, n2 );
		M::Vector4F		nd = _mm_hadd_ps( nd1, nd2 );
		
		// scale noise values
		M::Vector4F		noiseScale = _mm_set1_ps( 2.12f );
		M::vectorMul( n0, nd, noiseScale );
	}

};