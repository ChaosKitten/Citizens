#ifndef ZONE_HPP
#define ZONE_HPP

#include "MapTile.hpp"

namespace Citizens
{
	enum zone_scalar {
		M1 = -1,
		Z = 0,
		P1 = 1
	};
	struct zone_coord
	{
		zone_scalar x,y;
	};
	class Zone
	{
	public:
		void load(const irr::io::path& from_file);
		void unload(void);
		MapTile* get_tile(zone_coord at);
	private:
		// note: NULL tiles are possible in this array (in case of the 'current zone' being at the edge of the map, resulting in potentially <9 tiles)
		MapTile* tiles[9];
		
	};
};

#endif
