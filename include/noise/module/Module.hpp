#pragma once


// libnoise
#include "noise/module/ModuleBase.hpp"




namespace noise
{
	
	namespace module
	{
		
		template< typename ValueType, unsigned int Dimension >
		class Module;
		
		
		template< typename ValueT >
		class Module< ValueT, 1 >: public ModuleBase< Module< ValueT, 1 > >
		{
			//
			// Type aliases
			//
			public:
			
			typedef ValueT								ValueType;
			static const unsigned int					Dimension = 1;
			typedef Module< ValueType, Dimension >		ThisType;
			typedef ModuleBase< ThisType >				ModuleBaseType;
			
			
			
			//
			// Public interface
			//
			public:
			
			Module( int sourceModuleCount ):
			  ModuleBaseType( sourceModuleCount )
			{
			}
			
			virtual 
			~Module()
			{
			}
			
			virtual
			ValueType
			GetValue( ValueType x ) const = 0;

			virtual
			void
			GetValue4( const ValueType* inputX, ValueType* output ) const
			{
				for( uint8 i = 0; i < 4; ++i )
				{
					output[ i ] = GetValue( inputX[ i ] );
				}
			}
			
		};
		
		
		template< typename ValueT >
		class Module< ValueT, 2 >: public ModuleBase< Module< ValueT, 2 > >
		{
			//
			// Type aliases
			//
			public:
			
			typedef ValueT								ValueType;
			static const unsigned int					Dimension = 2;
			typedef Module< ValueType, Dimension >		ThisType;
			typedef ModuleBase< ThisType >				ModuleBaseType;
			
			
			
			//
			// Public interface
			//
			public:
			
			Module( int sourceModuleCount ):
			  ModuleBaseType( sourceModuleCount )
			{
			}
			
			virtual 
			~Module()
			{
			}
			
			virtual
			ValueType
			GetValue( ValueType x, ValueType y ) const = 0;

			virtual
			void
			GetValue4( const ValueType* inputX, const ValueType* inputY, ValueType* output ) const
			{
				for( uint8 i = 0; i < 4; ++i )
				{
					output[ i ] = GetValue( inputX[ i ], inputY[ i ] );
				}
			}
			
		};
		
		
		template< typename ValueT >
		class Module< ValueT, 3 >: public ModuleBase< Module< ValueT, 3 > >
		{
			//
			// Type aliases
			//
			public:
			
			typedef ValueT								ValueType;
			static const unsigned int					Dimension = 3;
			typedef Module< ValueType, Dimension >		ThisType;
			typedef ModuleBase< ThisType >				ModuleBaseType;
			
			
			
			//
			// Public interface
			//
			public:
			
			Module( int sourceModuleCount ):
			  ModuleBaseType( sourceModuleCount )
			{
			}
			
			virtual 
			~Module()
			{
			}
			
			virtual
			ValueType
			GetValue( ValueType x, ValueType y, ValueType z ) const = 0;

			virtual
			void
			GetValue4( const ValueType* inputX, const ValueType* inputY, const ValueType* inputZ, ValueType* output ) const
			{
				for( uint8 i = 0; i < 4; ++i )
				{
					output[ i ] = GetValue( inputX[ i ], inputY[ i ], inputZ[ i ] );
				}
			}
			
		};
		
		
		template< typename ValueT >
		class Module< ValueT, 4 >: public ModuleBase< Module< ValueT, 4 > >
		{
			//
			// Type aliases
			//
			public:
			
			typedef ValueT								ValueType;
			static const unsigned int					Dimension = 4;
			typedef Module< ValueType, Dimension >		ThisType;
			typedef ModuleBase< ThisType >				ModuleBaseType;
			
			
			
			//
			// Public interface
			//
			public:
			
			Module( int sourceModuleCount ):
			  ModuleBaseType( sourceModuleCount )
			{
			}
			
			virtual
			~Module()
			{
			}
			
			virtual
			ValueType
			GetValue( ValueType x, ValueType y, ValueType z, ValueType w ) const = 0;

			virtual
			void
			GetValue4( const ValueType* inputX, const ValueType* inputY, const ValueType* inputZ, const ValueType* inputW,
					   ValueType* output ) const
			{
				for( uint8 i = 0; i < 4; ++i )
				{
					output[ i ] = GetValue( inputX[ i ], inputY[ i ], inputZ[ i ], inputW[ i ] );
				}
			}
			
		};
		
	}
	
}
