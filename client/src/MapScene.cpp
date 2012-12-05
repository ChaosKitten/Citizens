#include "MapScene.hpp"
#include <sstream>
#include <string>

using namespace Citizens;

MapScene::MapScene(const Coordinate &c,irr::scene::ISceneManager& sm) : scenemgr(sm),ready(false),player_pos(c) {}

std::string terrain_texture(const Coordinate& player_pos)
{
	std::stringstream terrain_path;
	terrain_path << "terrain/zones/"
	<< player_pos.zone_x << "/"
	<< player_pos.zone_y << "/"
	<< "areas/"
	<< player_pos.area_x << "/"
	<< player_pos.area_y << "/"
	<< "texture.bmp";
	return terrain_path.str();
}

std::string terrain_heightmap(const Coordinate& player_pos)
{
	std::stringstream terrain_path;
	terrain_path << "terrain/zones/"
	<< player_pos.zone_x << "/"
	<< player_pos.zone_y << "/"
	<< "areas/"
	<< player_pos.area_x << "/"
	<< player_pos.area_y << "/"
	<< "heightmap.bmp";
	return terrain_path.str();
}

void MapScene::init(const Configuration& config)
{
	std::string heightmap = terrain_heightmap(player_pos);
	std::string texture = terrain_texture(player_pos);
	
	irr::scene::ICameraSceneNode* camera = scenemgr.addCameraSceneNodeFPS();
	scene_nodes["camera"] = camera;
	camera->bindTargetAndRotation(true);
	camera->setPosition(irr::core::vector3df(player_pos.pos_x,player_pos.pos_y,player_pos.pos_z));
	
	//! \todo rotate camera based on player's stored direction
	camera->setTarget(irr::core::vector3df(500.0f,0.0f,500.0f));
	
	irr::scene::ITerrainSceneNode* terrain = scenemgr.addTerrainSceneNode(
		irr::io::path(heightmap.c_str())
	);
	scene_nodes["terrain"] = terrain;
	terrain->setMaterialTexture(
		0,
		scenemgr.getVideoDriver()->getTexture(texture.c_str())
	);
	terrain->setScale(irr::core::vector3df(500.0f,500.0f,500.0f));
	terrain->setPosition(irr::core::vector3df(-250.0f,-250.0f,-10.0f));
	terrain->scaleTexture(5.0f);
	terrain->setMaterialFlag(irr::video::EMF_LIGHTING,false);
	
	irr::scene::ISceneNode* sky = scenemgr.addSkyDomeSceneNode(
		scenemgr.getVideoDriver()->getTexture("sky/dome.jpg"),
		16,
		16,
		1.0f,
		1.0f
	);
	scene_nodes["sky"] = sky;
	
	ready = true;
}

void MapScene::unload(void)
{
	for(scene_node_it i = scene_nodes.begin(); i != scene_nodes.end(); ++i)
	{
		i->second->remove();
	}
	scenemgr.getGUIEnvironment()->clear();
}

bool MapScene::is_setup(void)
{
	return ready;
}
