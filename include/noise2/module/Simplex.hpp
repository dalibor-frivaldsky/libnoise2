// ModuleBase
//
// Copyright (C) 2011 Dalibor Fr�valdsk�
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
#include <noise2/Platform.hpp>
#include <noise2/BasicTypes.hpp>




namespace noise2
{

	namespace module
	{

		template< typename ValueType, uint8 Dimension >
		class Simplex;


		namespace simplex
		{
			template< typename ValueType, uint8 Dimension, uint8 VectorSize >
			class SimplexImpl;
		}

	}

}



#include <noise2/module/simplex/Simplex_Scalar.hpp>
namespace noise2
{
	namespace module
	{
		template< typename ValueType, uint8 Dimension >
		class Simplex: public simplex::SimplexImpl< ValueType, Dimension, 1 >
		{
		};
	}
}
