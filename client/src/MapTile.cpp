#include "MapTile.hpp"

using namespace Citizens;

void MapTile::unload(void)
{
	std::map<Coordinate,Building::node_type*>::iterator i;
	for(i = buildings.begin(); i != buildings.end(); ++i)
	{
		i->second->remove();
	}
	buildings.clear();
	building_pool.clear();
	
}
