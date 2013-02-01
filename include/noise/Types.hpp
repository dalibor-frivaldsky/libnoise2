#pragma once


#include <BasicTypes.hpp>




namespace noise
{

	template< typename Precision, unsigned int Dimension >
	struct Vector;



#if defined( LIBNOISE2_USE_SSE )

	// Simd intrinsics
	//#include <xmmintrin.h>



	template<>
	struct Vector< float, 1 >
	{
		typedef float		Precision;
		typedef 1			Dimension;

		float		data;
	};

	template<>
	struct Vector< float, 2 >
	{
		typedef float		Precision;
		typedef 2			Dimension;

		__m128		data;
	};

	template<>
	struct Vector< float, 3 >
	{
		typedef float		Precision;
		typedef 3			Dimension;

		__m128		data;
	};

	template<>
	struct Vector< float, 4 >
	{
		typedef float		Precision;
		typedef 4			Dimension;

		__m128		data;
	};



	template<>
	struct Vector< int32, 1 >
	{
		typedef int32		Precision;
		typedef 1			Dimension;

		int32		data;
	};

	template<>
	struct Vector< int32, 2 >
	{
		typedef int32		Precision;
		typedef 2			Dimension;

		__m128i		data;
	};

	template<>
	struct Vector< int32, 3 >
	{
		typedef int32		Precision;
		typedef 3			Dimension;

		__m128i		data;
	};

	template<>
	struct Vector< int32, 4 >
	{
		typedef int32		Precision;
		typedef 4			Dimension;

		__m128i		data;
	};

#endif

}
