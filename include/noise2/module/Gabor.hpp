#pragma once


#include <noise2/Platform.hpp>
#include <noise2/BasicTypes.hpp>




namespace noise2
{

	namespace module
	{

		template< typename ValueType, uint8 Dimension >
		class Gabor;


		namespace gabor
		{
			template< typename ValueType, uint8 Dimension, uint8 VectorSize >
			class GaborImpl;
		}

	}

}

#if defined( LIBNOISE2_AT_LEAST_SSE2 )
#include <noise2/module/gabor/Gabor_Vector4.hpp>
namespace noise2
{
	namespace module
	{
		template< typename ValueType, uint8 Dimension >
		class Gabor: public gabor::GaborImpl< ValueType, Dimension, 4 >
		{
		};
	}
}
#else
#include <noise2/module/gabor/Gabor_Scalar.hpp>
namespace noise2
{
	namespace module
	{
		template< typename ValueType, uint8 Dimension >
		class Gabor: public gabor::GaborImpl< ValueType, Dimension, 1 >
		{
		};
	}
}
#endif
