#pragma once


#include <iostream>


#include <xmmintrin.h>


#include <noise/math/Math.hpp>




namespace noise
{

	namespace debug
	{

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

		void
		printVector( const math::Math< float >::Vector4I& v )
		{
			std::cout << "v = ";
			std::cout << ((unsigned int*)&v)[ 0 ];
			std::cout << ", " << ((unsigned int*)&v)[ 1 ];
			std::cout << ", " << ((unsigned int*)&v)[ 2 ];
			std::cout << ", " << ((unsigned int*)&v)[ 3 ];
			std::cout << std::endl;
		}

		void
		printVectorBool( const math::Math< float >::Vector4I& v )
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
