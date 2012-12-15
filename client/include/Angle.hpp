#ifndef ANGLE_HPP
#define ANGLE_HPP

#include <limits>

namespace Citizens
{
	/**
	 * \todo in the interests of making progress, I'm going to leave this class "broken" for now.
	 * the 'set' method needs to convert its test to static... somehow.
	 */
	template< typename T, int min, int max >
	class LimitedValue 
	{
	public:
		LimitedValue()
		{
			value = static_cast<T>(min);
		}
		
		template< int min2, int max2 >
		LimitedValue( const LimitedValue< T, min2, max2 > &other )
		{
			static_assert( min <= min2,"Value too low");
			static_assert( max >= max2,"Value too high");
			value = other.value;
		}
		
		bool set(const T v)
		{
			if(v > min && v < max)
			{
				value = v;
				return true;
			}
			return false;
		}
		T value;
	};
	typedef LimitedValue<float,0,360> NormalAngle;
	typedef LimitedValue<float,-180,180> RelativeAngle;
};


#endif
