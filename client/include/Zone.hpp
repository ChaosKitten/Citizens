#ifndef ZONE_HPP
#define ZONE_HPP

#include "CachedXML.hpp"
#include "MapTile.hpp"

namespace Citizens
{
	class zone_coord
	{
	public:
		zone_coord() { x = 0; y = 0; }
		zone_coord(short int x,short int y)
		{
			this->x = (0 < x) - (x < 0);
			this->y = (0 < y) - (y < 0);
		}
	private:
		short int x,y;
	};
	class Zone
	{
	public:
		void load(const CachedXML xml);
		void unload(void);
		MapTile* get_tile(zone_coord at);
	private:
		// note: NULL tiles are possible in this array (in case of the 'current zone' being at the edge of the map, resulting in potentially <9 tiles)
		MapTile* tiles[9];
		
	};
};

#endif
