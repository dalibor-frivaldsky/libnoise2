#pragma once


#include <iostream>
#include <bitset>


#include <xmmintrin.h>


#include <noise2/math/Math.hpp>




namespace noise2
{

	namespace debug
	{

		static inline
		void
		printVector( const math::Math< float >::Vector4F& v )
		{
			std::cout << "v = ";
			std::cout << ((float*)&v)[ 0 ];
			std::cout << ", " << ((float*)&v)[ 1 ];
			std::cout << ", " << ((float*)&v)[ 2 ];
			std::cout << ", " << ((float*)&v)[ 3 ];
			std::cout << std::endl;
		}

		static inline
		void
		printVector( const math::Math< double >::Vector4F& v )
		{
			std::cout << "v = ";
			std::cout << ((double*)&v.lo)[ 0 ];
			std::cout << ", " << ((double*)&v.lo)[ 1 ];
			std::cout << ", " << ((double*)&v.hi)[ 0 ];
			std::cout << ", " << ((double*)&v.hi)[ 1 ];
			std::cout << std::endl;
		}

		static inline
		void
		printVector( const math::Math_SSE2_Integer< float >::Vector4I& v )
		{
			std::cout << "v = ";
			std::cout << ((unsigned int*)&v)[ 0 ];
			std::cout << ", " << ((unsigned int*)&v)[ 1 ];
			std::cout << ", " << ((unsigned int*)&v)[ 2 ];
			std::cout << ", " << ((unsigned int*)&v)[ 3 ];
			std::cout << std::endl;
		}

		static inline
		void
		printVectorInt( const math::Math_SSE2_Integer< float >::Vector4I& v )
		{
			std::cout << "v = ";
			std::cout << ((int*)&v)[ 0 ];
			std::cout << ", " << ((int*)&v)[ 1 ];
			std::cout << ", " << ((int*)&v)[ 2 ];
			std::cout << ", " << ((int*)&v)[ 3 ];
			std::cout << std::endl;
		}

		static inline
		void
		printVectorBin( const math::Math_SSE2_Integer< float >::Vector4I& v )
		{
			std::cout << "v = ";
			std::cout << std::bitset< 32 >( ((unsigned int*)&v)[ 0 ] );
			std::cout << ", " << ((int*)&v)[ 1 ];
			std::cout << ", " << ((int*)&v)[ 2 ];
			std::cout << ", " << ((int*)&v)[ 3 ];
			std::cout << std::endl;
		}

		static inline
		void
		printVectorBool( const math::Math_SSE2_Integer< float >::Vector4I& v )
		{
			std::cout << "v = ";
			std::cout << (((unsigned int*)&v)[ 0 ] == 0xffffffff ? 1 : 0);
			std::cout << ", " << (((unsigned int*)&v)[ 1 ] == 0xffffffff ? 1 : 0);
			std::cout << ", " << (((unsigned int*)&v)[ 2 ] == 0xffffffff ? 1 : 0);
			std::cout << ", " << (((unsigned int*)&v)[ 3 ] == 0xffffffff ? 1 : 0);
			std::cout << std::endl;
		}

	}

}
