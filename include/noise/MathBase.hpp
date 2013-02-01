namespace noise
{

	template< typename ValueType >
	class MathBase
	{

	public:
		
		static inline
		ValueType
		Pi()
		{
			return ValueType( 3.14159265358979323846 );
		}

		static inline
		ValueType
		floor( ValueType x )
		{
			return ( x > ValueType( 0.0 ) ? (int) x: (int) x - 1 );
		}
	
	};

}
