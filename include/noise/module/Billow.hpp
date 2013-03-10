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


// libnoise
#include <noise/Platform.hpp>
#include <noise/BasicTypes.hpp>




namespace noise
{

	namespace module
	{

		template< typename ValueType, uint8 Dimension >
		class Billow;


		namespace perlin
		{
			template< typename ValueType, uint8 Dimension, uint8 VectorSize >
			class BillowImpl;
		}

	}

}



#if defined( LIBNOISE2_AT_LEAST_SSE2 )
#include <noise/module/perlin/Billow_Vector4.hpp>
namespace noise
{
	namespace module
	{
		template< typename ValueType, uint8 Dimension >
		class Billow: public perlin::BillowImpl< ValueType, Dimension, 4 >
		{
		};
	}
}
#else
#include <noise/module/perlin/Billow_Scalar.hpp>
namespace noise
{
	namespace module
	{
		template< typename ValueType, uint8 Dimension >
		class Billow: public perlin::BillowImpl< ValueType, Dimension, 1 >
		{
		};
	}
}
#endif
