#include "MapTile.hpp"

using namespace Citizens;

void MapTile::unload(void)
{
	std::map<Coordinate,Building>::iterator i;
	for(i = buildings.begin(); i != buildings.end(); ++i)
	{
		i->second.unload();
	}
}
