#pragma once


#include <noise2/module/Module.hpp>




namespace noise2
{

	namespace module
	{

		template< typename ValueT, unsigned int Dimension, typename AdaptedT, unsigned int AdapteeDimension >
		class DimensionAdapter;

		template< typename ValueT, typename AdaptedT >
		class DimensionAdapter< ValueT, 2, AdaptedT, 1 >: public Module< ValueT, 2 >
		{

		public:

			typedef ValueT								ValueType;
			typedef AdaptedT							AdaptedType;
			static const unsigned						Dimension = 2;
			typedef Module< ValueType, Dimension >		ModuleType;
			typedef DimensionAdapter< ValueType, Dimension, AdaptedType, 1 >		ThisType;



		private:

			const AdaptedType&		adaptee;



		public:

			DimensionAdapter( const AdaptedType& adaptee ):
			  ModuleType( 0 ),
			  adaptee( adaptee )
			{}

			virtual
			~DimensionAdapter()
			{}

			virtual
			ValueType
			GetValue( ValueType x, ValueType y ) const
			{
				return adaptee.GetValue( x );
			}

		};

		template< typename ValueT, typename AdaptedT >
		class DimensionAdapter< ValueT, 2, AdaptedT, 2 >: public Module< ValueT, 2 >
		{

		public:

			typedef ValueT								ValueType;
			typedef AdaptedT							AdaptedType;
			static const unsigned						Dimension = 2;
			typedef Module< ValueType, Dimension >		ModuleType;
			typedef DimensionAdapter< ValueType, Dimension, AdaptedType, 2 >		ThisType;



		private:

			const AdaptedType&		adaptee;



		public:

			DimensionAdapter( const AdaptedType& adaptee ):
			  ModuleType( 0 ),
			  adaptee( adaptee )
			{}

			virtual
			~DimensionAdapter()
			{}

			virtual
			ValueType
			GetValue( ValueType x, ValueType y ) const
			{
				return adaptee.GetValue( x, y );
			}

		};

		template< typename ValueT, typename AdaptedT >
		class DimensionAdapter< ValueT, 2, AdaptedT, 3 >: public Module< ValueT, 2 >
		{

		public:

			typedef ValueT								ValueType;
			typedef AdaptedT							AdaptedType;
			static const unsigned						Dimension = 2;
			typedef Module< ValueType, Dimension >		ModuleType;
			typedef DimensionAdapter< ValueType, Dimension, AdaptedType, 3 >		ThisType;



		private:

			const AdaptedType&		adaptee;



		public:

			DimensionAdapter( const AdaptedType& adaptee ):
			  ModuleType( 0 ),
			  adaptee( adaptee )
			{}

			virtual
			~DimensionAdapter()
			{}

			virtual
			ValueType
			GetValue( ValueType x, ValueType y ) const
			{
				return adaptee.GetValue( x, y, ValueType( 0.0 ) );
			}

		};

		template< typename ValueT, typename AdaptedT >
		class DimensionAdapter< ValueT, 2, AdaptedT, 4 >: public Module< ValueT, 2 >
		{

		public:

			typedef ValueT								ValueType;
			typedef AdaptedT							AdaptedType;
			static const unsigned						Dimension = 2;
			typedef Module< ValueType, Dimension >		ModuleType;
			typedef DimensionAdapter< ValueType, Dimension, AdaptedType, 4 >		ThisType;



		private:

			const AdaptedType&		adaptee;



		public:

			DimensionAdapter( const AdaptedType& adaptee ):
			  ModuleType( 0 ),
			  adaptee( adaptee )
			{}

			virtual
			~DimensionAdapter()
			{}

			virtual
			ValueType
			GetValue( ValueType x, ValueType y ) const
			{
				return adaptee.GetValue( x, y, ValueType( 0.0 ), ValueType( 0.0 ) );
			}

		};

	}

}
