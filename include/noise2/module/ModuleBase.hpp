#pragma once


// Std C++
#include <vector>




namespace noise2
{
	
	namespace module
	{
		
		template< typename ModuleType >
		class ModuleBase
		{
			
			//
			// Public interface
			//
			public:
			
			ModuleBase( int sourceModuleCount )
			{
				sourceModules.resize( sourceModuleCount );
			}
			
			virtual
			~ModuleBase()
			{
			}
			
			
			int
			GetSourceModuleCount () const
			{
				return sourceModules.size();
			}
			
			const ModuleType&
			GetSourceModule( int index ) const
		    {
				if( index >= GetSourceModuleCount () || index < 0 )
				{
					// throw noise::ExceptionNoModule (); TODO throw exception
				}
				
				return *( sourceModules[ index ] );
		    }
			
			void 
			SetSourceModule( int index, const ModuleType& sourceModule )
			{
				if( index >= GetSourceModuleCount () || index < 0 )
				{
					// throw noise::ExceptionInvalidParam ();	TODO throw expception
				}
				
				sourceModules[ index ] = &sourceModule;
			}
			
			
			
			//
			// Private interface
			//
			private:
			
			ModuleBase( const ModuleBase& m )
			{
			}
			
			
			
			//
			// Fields
			//
			private:
			
			std::vector< ModuleType* >		sourceModules;
			
		};
		
	}	
	
}
