#ifndef COORDINATE_HPP
#define COORDINATE_HPP

/* until we can get Angle working properly, let's just not bother.
#include "Angle.hpp"
*/

namespace Citizens
{
	struct Coordinate
	{
		int area_x;
		int area_y;
		int zone_x;
		int zone_y;
		int pos_x;
		int pos_y;
		int pos_z;
		float dir_x;
		float dir_y;
		float dir_z;
	};
};


#endif
