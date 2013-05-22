#pragma once


#include <cmath>


#include <xmmintrin.h>


#include <noise/math/MathBase.hpp>




#define LIBNOISE2_SUPPORT_CONST_ARRAY( type, name, value )	static const VECTOR4_ALIGN( type name[ 4 ] ) = { value, value, value, value }



namespace noise
{

	namespace math
	{

		template< typename ValueType >
		class Math_SSE1;



		template<>
		class Math_SSE1< float >: public MathBase< float >
		{

		public:
		
			typedef __m128			Vector4F;



		public:

			// Memory operations
			static inline
			Vector4F
			loadFromMemory( const ScalarF* memoryLocation )
			{
				return _mm_load_ps( memoryLocation );
			}

			static inline
			void
			storeToMemory( ScalarF* memoryLocation, const Vector4F& v )
			{
				_mm_store_ps( memoryLocation, v );
			}


			// Constants
			static inline
			Vector4F
			constZeroF()
			{
				return _mm_setzero_ps();
			}

			static inline
			Vector4F
			constHalfF()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, halfA, 0.5f );
				return loadFromMemory( halfA );
			}

			static inline
			Vector4F
			constOneF()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, oneA, 1.0f );
				return loadFromMemory( oneA );
			}

			static inline
			Vector4F
			constMinusOneF()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, minusOneA, -1.0f );
				return loadFromMemory( minusOneA );
			}

			static inline
			Vector4F
			constTwoF()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, twoA, 2.0f );
				return loadFromMemory( twoA );
			}

			static inline
			Vector4F
			constPiF()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, piA, 3.14159265358979323846 );
				return loadFromMemory( piA );
			}

			static inline
			Vector4F
			constMinusPiF()
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( float, minusPiA, -3.14159265358979323846 );
				return loadFromMemory( minusPiA );
			}


			// Vectorization
			static inline
			Vector4F
			vectorizeOne( const ScalarF& f )
			{
				return _mm_set1_ps( f );
			}
		
			static inline
			Vector4F
			vectorize( const ScalarF& x, const ScalarF& y )
			{
				return _mm_set_ps( 0.0f, 0.0f, y, x );
			}
		
			static inline
			Vector4F
			vectorize( const ScalarF& x, const ScalarF& y, const ScalarF& z )
			{
				return _mm_set_ps( 0.0f, z, y, x );
			}
		
			static inline
			Vector4F
			vectorize( const ScalarF& x, const ScalarF& y, const ScalarF& z, const ScalarF& w )
			{
				return _mm_set_ps( w, z, y, x );
			}


			// Extraction
			static inline
			ScalarF
			extract1st( const Vector4F& v )
			{
				ScalarF	ret;
				
				_mm_store_ss( &ret, v );

				return ret;
			}


			// Shuffle operations
			template< uint8 First, uint8 Second, uint8 Third, uint8 Fourth >
			static inline
			Vector4F
			shuffle( const Vector4F& a, const Vector4F& b )
			{
				return _mm_shuffle_ps( a, b, _MM_SHUFFLE( Fourth, Third, Second, First ) );
			}

			template< typename OrderPolicy >
			static inline
			Vector4F
			shuffle( const Vector4F& a, const Vector4F& b )
			{
				return _mm_shuffle_ps( a, b, _MM_SHUFFLE( OrderPolicy::o4, OrderPolicy::o3, OrderPolicy::o2, OrderPolicy::o1 ) );
			}


			// Interleave operations
			static inline
			Vector4F
			interleaveHi( const Vector4F& a, const Vector4F& b )
			{
				return _mm_unpackhi_ps( a, b );
			}

			static inline
			Vector4F
			interleaveLo( const Vector4F& a, const Vector4F& b )
			{
				return _mm_unpacklo_ps( a, b );
			}


			// Arithmetic operations
			static inline
			Vector4F
			multiply( const Vector4F& l, const Vector4F& r )
			{
				return _mm_mul_ps( l, r );
			}

			static inline
			Vector4F
			divide( const Vector4F& l, const Vector4F& r )
			{
				return _mm_div_ps( l, r );
			}
		
			static inline
			Vector4F
			add( const Vector4F& l, const Vector4F& r )
			{
				return _mm_add_ps( l, r );
			}

			static inline
			Vector4F
			subtract( const Vector4F& l, const Vector4F& r )
			{
				return _mm_sub_ps( l, r );
			}

			static inline
			Vector4F
			reduce( const Vector4F& l, const Vector4F& r )
			{
				Vector4F	tmp1 = shuffle< 0, 2, 0, 2 >( l, r );
				Vector4F	tmp2 = shuffle< 1, 3, 1, 3 >( l, r );

				return add( tmp1, tmp2 );
			}

			static inline
			Vector4F
			abs( const Vector4F& v )
			{
				LIBNOISE2_SUPPORT_CONST_ARRAY( int32, invSignMaskA, ~0x80000000 );
				Vector4F	invSignMaskV = loadFromMemory( (ScalarF*) invSignMaskA );

				return _mm_and_ps( v, invSignMaskV );
			}

			static inline
			Vector4F
			sqrtRecip( const Vector4F& v )
			{
				return _mm_sqrt_ps( v );
			}

			// Comparison
			static inline
			Vector4F
			equal( const Vector4F& l, const Vector4F& r )
			{
				return _mm_cmpeq_ps( l, r );
			}

			static inline
			Vector4F
			greaterThan( const Vector4F& l, const Vector4F& r )
			{
				return _mm_cmpgt_ps( l, r );
			}

			static inline
			Vector4F
			equalGreaterThan( const Vector4F& l, const Vector4F& r )
			{
				return _mm_cmpge_ps( l, r );
			}

			static inline
			Vector4F
			lowerThan( const Vector4F& l, const Vector4F& r )
			{
				return _mm_cmplt_ps( l, r );
			}


			// Shift operations
			static inline
			Vector4F
			shiftRightLoop( const Vector4F& v, int count )
			{
				Vector4F	shiftedV = v;

				for( int i = 0; i < count; ++i )
				{
					shiftedV = shuffle< 3, 0, 1, 2 >( shiftedV, shiftedV );
				}

				return shiftedV;
			}

			static inline
			Vector4F
			shiftLeftLoop( const Vector4F& v, int count )
			{
				Vector4F	shiftedV = v;

				for( int i = 0; i < count; ++i )
				{
					shiftedV = shuffle< 1, 2, 3, 0 >( shiftedV, shiftedV );
				}

				return shiftedV;
			}


			// Distribution operations
			static inline
			Vector4F
			distribute1st( const Vector4F& input )
			{
				return _mm_shuffle_ps( input, input, _MM_SHUFFLE( 0, 0, 0, 0 ) );
			}

			static inline
			Vector4F
			distribute2nd( const Vector4F& input )
			{
				return _mm_shuffle_ps( input, input, _MM_SHUFFLE( 1, 1, 1, 1 ) );
			}

			static inline
			Vector4F
			distribute3rd( const Vector4F& input )
			{
				return _mm_shuffle_ps( input, input, _MM_SHUFFLE( 2, 2, 2, 2 ) );
			}

			static inline
			Vector4F
			distribute4th( const Vector4F& input )
			{
				return _mm_shuffle_ps( input, input, _MM_SHUFFLE( 3, 3, 3, 3 ) );
			}

		};



		template<>
		class Math_SSE1< double >: public MathBase< double >
		{

		public:

			// Memory operations
			static inline void	loadFromMemory();
			static inline void	storeToMemory();


			// Vectorization
			static inline void	vectorizeOne();
			static inline void	vectorize();
			

			// Shuffle operations
			static inline void	shuffle();


			// Interleave operations
			static inline void interleaveHi();
			static inline void interleaveLo();
			

			// Arithmetic operations
			static inline void	multiply();
			static inline void	divide();
			static inline void	add();
			static inline void	subtract();


			// Comparison
			static inline void	equal();
			static inline void	greaterThan();
			static inline void	lowerThan();


			// Shift operations
			static inline void	shiftRightLoop();
			static inline void	shiftLeftLoop();


			// Distribution operations
			static inline void	distribute1st();
			static inline void	distribute2nd();
			static inline void	distribute3rd();
			static inline void	distribute4th();

		};

	}

}
