#pragma once

#include <noise/Platform.hpp>




namespace noise
{
	
	template< typename ValueType >
	class Math;
	
}

#if defined( LIBNOISE2_AT_LEAST_SSE2 )
	#include "noise/Math_SSE.hpp"
#else
	#include "noise/Math_No_SSE.hpp"
#endif
