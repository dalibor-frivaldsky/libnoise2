// ModuleBase
//
// Copyright (C) 2011 Dalibor Frívaldský
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once


// Std C++
#include <cmath>
#include <cstring>
#include <iostream>


// libnoise
#include "noise/Platform.hpp"
#include "noise/Math.hpp"
#include "noise/Interpolations.hpp"
#include "noise/VectorTable.hpp"
#include "noise/PermutationTable.hpp"



namespace noise
{

	enum NoiseQuality
	{
		/// Generates coherent noise quickly.  When a coherent-noise function with
		/// this quality setting is used to generate a bump-map image, there are
		/// noticeable "creasing" artifacts in the resulting image.  This is
		/// because the derivative of that function is discontinuous at integer
		/// boundaries.
		QUALITY_FAST = 0,

		/// Generates standard-quality coherent noise.  When a coherent-noise
		/// function with this quality setting is used to generate a bump-map
		/// image, there are some minor "creasing" artifacts in the resulting
		/// image.  This is because the second derivative of that function is
		/// discontinuous at integer boundaries.
		QUALITY_STD = 1,

		/// Generates the best-quality coherent noise.  When a coherent-noise
		/// function with this quality setting is used to generate a bump-map
		/// image, there are no "creasing" artifacts in the resulting image.  This
		/// is because the first and second derivatives of that function are
		/// continuous at integer boundaries.
		QUALITY_BEST = 2
	};
	
	
	
	
	const int X_NOISE_GEN = 1619;
	const int Y_NOISE_GEN = 31337;
	const int Z_NOISE_GEN = 6971;
	const int W_NOISE_GEN = 159869;
	const int SEED_NOISE_GEN = 1013;
	const int SHIFT_NOISE_GEN = 8;
	
	
	
	
	template< typename ValueType, unsigned int Dimension >
	class NoiseGen;
	
	
	
	#include "noise/NoiseGen_1.hpp"

	#if defined( LIBNOISE2_AT_LEAST_SSE2 )
	#include "noise/NoiseGen_2F_SSE_2.hpp"
	#include "noise/NoiseGen_3F_SSE_2.hpp"
	#include "noise/NoiseGen_4F_SSE_2.hpp"

	#elif defined( LIBNOISE2_USE_SSE )
	#if defined( LIBNOISE2_USE_SSE_2 )
	#include "noise/NoiseGen_2F_SSE_2.hpp"
	#include "noise/NoiseGen_3F_SSE_2.hpp"
	#include "noise/NoiseGen_4F_SSE_2.hpp"
	#else
	#include "noise/NoiseGen_2F_SSE.hpp"
	#include "noise/NoiseGen_3F_SSE.hpp"
	#include "noise/NoiseGen_4F_SSE.hpp"
	#endif

	#else
	#include "noise/NoiseGen_2.hpp"
	#include "noise/NoiseGen_3.hpp"
	#include "noise/NoiseGen_4.hpp"
	#endif
}
