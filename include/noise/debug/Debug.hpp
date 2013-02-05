#pragma once


#include <iostream>


#include <xmmintrin.h>




namespace noise
{

	namespace debug
	{

		void
		printVector( const __m128& v )
		{
			std::cout << "v = ";
			std::cout << ((float*)&v)[ 0 ];
			std::cout << ", " << ((float*)&v)[ 1 ];
			std::cout << ", " << ((float*)&v)[ 2 ];
			std::cout << ", " << ((float*)&v)[ 3 ];
			std::cout << std::endl;
		}

		void
		printVector( const __m128i& v )
		{
			std::cout << "v = ";
			std::cout << ((unsigned int*)&v)[ 0 ];
			std::cout << ", " << ((unsigned int*)&v)[ 1 ];
			std::cout << ", " << ((unsigned int*)&v)[ 2 ];
			std::cout << ", " << ((unsigned int*)&v)[ 3 ];
			std::cout << std::endl;
		}

		void
		printVectorBool( const __m128i& v )
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
