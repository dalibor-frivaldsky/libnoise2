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
#include <noise2/Platform.hpp>



namespace noise2
{

	namespace module
	{

		namespace perlin
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
			
			
			
			enum NoiseGenData
			{
				X_NOISE_GEN = 1619,
				Y_NOISE_GEN = 31337,
				Z_NOISE_GEN = 6971,
				W_NOISE_GEN = 159869,
				SEED_NOISE_GEN = 1013,
				SHIFT_NOISE_GEN = 8
			};
			
			
			
			template< typename ValueType, uint8 Dimension >
			class NoiseGen;

			template< typename ValueType, uint8 Dimension, uint8 VectorSize >
			class NoiseGenImpl;

		}

	}
	
}



#if defined( LIBNOISE2_AT_LEAST_SSE2 )
#include <noise2/module/perlin/NoiseGen_Vector4.hpp>
namespace noise2
{
	namespace module
	{
		namespace perlin
		{
			template< typename ValueType, uint8 Dimension >
			class NoiseGen: public perlin::NoiseGenImpl< ValueType, Dimension, 4 >
			{
			};
		}
	}
}
#else
#include <noise2/module/perlin/NoiseGen_Scalar.hpp>
namespace noise2
{
	namespace module
	{
		namespace perlin
		{
			template< typename ValueType, uint8 Dimension >
			class NoiseGen: public perlin::NoiseGenImpl< ValueType, Dimension, 1 >
			{
			};
		}
	}
}
#endif
