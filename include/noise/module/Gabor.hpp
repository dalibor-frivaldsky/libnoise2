#pragma once


#include <noise/Platform.hpp>
#include <noise/BasicTypes.hpp>




namespace noise
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
#include <noise/module/gabor/Gabor_Vector4.hpp>
namespace noise
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
#include <noise/module/gabor/Gabor_Scalar.hpp>
namespace noise
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
