#pragma once


// libnoise
#include "noise/Math.hpp"




namespace noise
{
	
	template< typename ValueType >
	class Interpolations;
	
}

#if defined( LIBNOISE2_USE_SSE )
	#include "noise/Interpolations_SSE.hpp"
#else
	#include "noise/Interpolations_No_SSE.hpp"
#endif
