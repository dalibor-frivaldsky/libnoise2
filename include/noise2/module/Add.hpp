#pragma once


// libnoise
#include "noise2/module/Module.hpp"




namespace noise2
{
	
	namespace module
	{
		
		template< typename ValueType, unsigned int Dimension >
		class Add;
		
		
		template< typename ValueT >
		class Add< ValueT, 1 >: public Module< ValueT, 1 >
		{
			
			//
			// Type aliases
			//
			public:
			
			typedef ValueT								ValueType;
			static const unsigned int					Dimension = 1;
			typedef Module< ValueType, Dimension >		ModuleType;
			
			
			
			//
			// Public interface
			//
			public:
			
			Add():
			ModuleType( 2 )
			{
			}
			
			virtual
			~Add()
			{
			}
			
			virtual
			ValueType
			GetValue( ValueType x ) const
			{
				return GetSourceModule( 0 ).GetValue( x ) + GetSourceModule( 1 ).GetValue( x );
			}
			
		};
		
		
		template< typename ValueT >
		class Add< ValueT, 2 >: public Module< ValueT, 2 >
		{
			
			//
			// Type aliases
			//
			public:
			
			typedef ValueT								ValueType;
			static const unsigned int					Dimension = 2;
			typedef Module< ValueType, Dimension >		ModuleType;
			
			
			
			//
			// Public interface
			//
			public:
			
			Add():
			ModuleType( 2 )
			{
			}
			
			virtual
			~Add()
			{
			}
			
			virtual
			ValueType
			GetValue( ValueType x, ValueType y ) const
			{
				return GetSourceModule( 0 ).GetValue( x, y ) + GetSourceModule( 1 ).GetValue( x, y );
			}
			
		};
		
		
		template< typename ValueT >
		class Add< ValueT, 3 >: public Module< ValueT, 3 >
		{
			
			//
			// Type aliases
			//
			public:
			
			typedef ValueT								ValueType;
			static const unsigned int					Dimension = 3;
			typedef Module< ValueType, Dimension >		ModuleType;
			
			
			
			//
			// Public interface
			//
			public:
			
			Add():
			ModuleType( 2 )
			{
			}
			
			virtual
			~Add()
			{
			}
			
			virtual
			ValueType
			GetValue( ValueType x, ValueType y, ValueType z ) const
			{
				return GetSourceModule( 0 ).GetValue( x, y, z ) + GetSourceModule( 1 ).GetValue( x, y, z );
			}
			
		};
		
		
		template< typename ValueT >
		class Add< ValueT, 4 >: public Module< ValueT, 4 >
		{
			
			//
			// Type aliases
			//
			public:
			
			typedef ValueT								ValueType;
			static const unsigned int					Dimension = 4;
			typedef Module< ValueType, Dimension >		ModuleType;
			
			
			
			//
			// Public interface
			//
			public:
			
			Add():
			ModuleType( 2 )
			{
			}
			
			virtual
			~Add()
			{
			}
			
			virtual
			ValueType
			GetValue( ValueType x, ValueType y, ValueType z, ValueType w ) const
			{
				return GetSourceModule( 0 ).GetValue( x, y, z, w ) + GetSourceModule( 1 ).GetValue( x, y, z, w );
			}
			
		};
		
	}
	
}
