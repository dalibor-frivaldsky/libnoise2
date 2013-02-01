#pragma once

#include <noise/Platform.hpp>




namespace noise
{

	namespace module
	{

		template< typename ValueT, unsigned Dimension >
		class Gabor;

	}

}

#if defined( LIBNOISE2_AT_LEAST_SSE2 )
#include <noise/module/gabor/Gabor_SSE2.hpp>
#else
#include <noise/module/gabor/Gabor_No_SSE.hpp>
#endif
