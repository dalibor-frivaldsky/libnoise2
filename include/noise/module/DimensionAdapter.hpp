#pragma once


#include <noise/module/Module.hpp>




namespace noise
{

	namespace module
	{

		template< typename ValueT, unsigned int Dimension, typename AdaptedT, unsigned int AdapteeDimension >
		class DimensionAdapter;

		template< typename ValueT, typename AdaptedT >
		class DimensionAdapter< ValueT, 2, AdaptedT, 1 >: public Module< ValueT, 2 >, private AdaptedT
		{

		public:

			typedef ValueT								ValueType;
			typedef AdaptedT							AdaptedType;
			static const unsigned						Dimension = 2;
			typedef Module< ValueType, Dimension >		ModuleType;
			typedef DimensionAdapter< ValueType, Dimension, AdaptedType, 1 >		ThisType;



		public:

			DimensionAdapter():
			  ModuleType( 0 )
			{}

			virtual
			~DimensionAdapter()
			{}

			virtual
			ValueType
			GetValue( ValueType x, ValueType y ) const
			{
				return AdaptedType::GetValue( x );
			}

		};

		template< typename ValueT, typename AdaptedT >
		class DimensionAdapter< ValueT, 2, AdaptedT, 3 >: public Module< ValueT, 2 >, private AdaptedT
		{

		public:

			typedef ValueT								ValueType;
			typedef AdaptedT							AdaptedType;
			static const unsigned						Dimension = 2;
			typedef Module< ValueType, Dimension >		ModuleType;
			typedef DimensionAdapter< ValueType, Dimension, AdaptedType, 3 >		ThisType;



		public:

			DimensionAdapter():
			  ModuleType( 0 )
			{}

			virtual
			~DimensionAdapter()
			{}

			virtual
			ValueType
			GetValue( ValueType x, ValueType y ) const
			{
				return AdaptedType::GetValue( x, y, ValueType( 0.0 ) );
			}

		};

		template< typename ValueT, typename AdaptedT >
		class DimensionAdapter< ValueT, 2, AdaptedT, 4 >: public Module< ValueT, 2 >, private AdaptedT
		{

		public:

			typedef ValueT								ValueType;
			typedef AdaptedT							AdaptedType;
			static const unsigned						Dimension = 2;
			typedef Module< ValueType, Dimension >		ModuleType;
			typedef DimensionAdapter< ValueType, Dimension, AdaptedType, 4 >		ThisType;



		public:

			DimensionAdapter():
			  ModuleType( 0 )
			{}

			virtual
			~DimensionAdapter()
			{}

			virtual
			ValueType
			GetValue( ValueType x, ValueType y ) const
			{
				return AdaptedType::GetValue( x, y, ValueType( 0.0 ), ValueType( 0.0 ) );
			}

		};

	}

}
