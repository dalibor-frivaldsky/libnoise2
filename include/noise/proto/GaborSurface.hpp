#pragma once


#include <noise/Platform.hpp>
#include <noise/BasicTypes.hpp>




namespace noise
{

	namespace proto
	{

		template< typename ValueType >
		class GaborSurface;


		namespace gaborSurface
		{
			template< typename ValueType, uint8 VectorSize >
			class GaborSurfaceImpl;
		}

	}

}

#if defined( LIBNOISE2_AT_LEAST_SSE2 )
#include <noise/proto/gaborSurface/GaborSurface_Vector4.hpp>
namespace noise
{
	namespace proto
	{
		template< typename ValueType >
		class GaborSurface: public gaborSurface::GaborSurfaceImpl< ValueType, 4 >
		{
		};
	}
}
#else
#include <noise/proto/gaborSurface/GaborSurface_Scalar.hpp>
namespace noise
{
	namespace proto
	{
		template< typename ValueType >
		class GaborSurface: public gaborSurface::GaborSurfaceImpl< ValueType, 1 >
		{
		};
	}
}
#endif
