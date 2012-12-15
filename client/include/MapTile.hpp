#ifndef MAPTILE_HPP
#define MAPTILE_HPP

#include "Coordinate.hpp"
#include "Building.hpp"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <irrlicht.h>
#pragma GCC diagnostic pop

#include <map>
#include <string>

namespace Citizens
{
	class MapTile
	{
	public:
		MapTile();
		MapTile(const MapTile& mt);
		void load(const irr::io::path& from_file);
		// split render from load
		void render();
		void unload(void);
	private:
		// describes all the buildings available to the map tile for rendering
		std::map<std::string,Building> building_pool;
		// a texture to place over the terrain
		irr::video::ITexture* terrain_splat;
		// all the buildings that occupy this particular map tile.
		std::map<Coordinate,Building::node_type*> buildings;
	};

};

#endif
