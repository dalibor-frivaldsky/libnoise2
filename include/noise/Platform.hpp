#pragma once




#if defined( __GNUC__ )
#define SSE_ALIGN( v )			v __attribute__ ((aligned (16)))
#elif defined( _MSC_VER )
#define SSE_ALIGN( v )			__declspec( align( 16 ) ) v
#endif

#if defined( __GNUC__ )
#define VECTOR_ALIGN( v )			v __attribute__ ((aligned (16)))
#elif defined( _MSC_VER )
#define VECTOR_ALIGN( v )			__declspec( align( 16 ) ) v
#endif


#if defined( LIBNOISE2_USE_SSE1 )
#define LIBNOISE2_AT_LEAST_SSE1
#endif

#if defined( LIBNOISE2_USE_SSE2 )
#define LIBNOISE2_AT_LEAST_SSE1
#define LIBNOISE2_AT_LEAST_SSE2
#endif

#if defined( LIBNOISE2_USE_SSE2 )
#define LIBNOISE2_AT_LEAST_SSE1
#define LIBNOISE2_AT_LEAST_SSE2
#define LIBNOISE2_AT_LEAST_SSE3
#endif
