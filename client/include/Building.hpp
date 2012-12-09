#ifndef BUILDING_HPP
#define BUILDING_HPP

#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <irrlicht.h>
#pragma GCC diagnostic pop
#include <string>
#include <map>

namespace Citizens
{

	class Building
	{
	public:
		Building(
			irr::scene::ISceneManager& sm,
			irr::io::path af
		);

		irr::scene::IAnimatedMeshSceneNode* get_mesh_node(void);
		void move_to(const irr::core::vector3df& new_pos);
		void move(const irr::core::vector3df& direction, unsigned int amount);
		void turn_to(const irr::core::vector3df& new_orient);
		void turn(const irr::core::vector3df& axes, float angle_deg);
		void turn_r(const irr::core::vector3df& axes, float angle_rad);
		void reload(bool and_reset = true);
		irr::io::path get_asset_file(void) { return asset_file; }
		
	private:
		void reset(void);
		
		
		irr::io::path asset_file;
		irr::scene::ISceneManager& sm;
		
		std::string name;
		std::map<irr::core::stringw,irr::io::path> skins;
		irr::io::path model;
		irr::core::vector3df position;
		irr::core::vector3df orientation;
		irr::core::vector3df scale;
		irr::scene::IAnimatedMeshSceneNode* mesh_node;
	};

};

#endif
