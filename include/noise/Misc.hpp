#pragma once




namespace noise
{

	template< typename ValueType >
	inline
	ValueType
	ClampValue( ValueType value, ValueType lowerBound, ValueType upperBound )
	{
		if( value < lowerBound )
		{
			return lowerBound;
		}
		else if( value > upperBound )
		{
			return upperBound;
		}
		else
		{
			return value;
		}
	}

}