#include "MapScene.hpp"
#include <sstream>
#include <string>
#include <map>

#ifdef __linux
#include <sys/inotify.h>
#include <thread>
#elif _WIN32 || _WIN64
#warning "Dynamic reloading of objects disabled on this platform"
#else
#warning "Unsupported platform, dynamic reloading of objects disabled"
#endif

#include <iostream>

using namespace Citizens;

MapScene::MapScene(GraphicsEngine& ge,const Coordinate &c,irr::scene::ISceneManager& sm,irr::IrrlichtDevice& d) : scenemgr(sm),engine(ge),ready(false),player_pos(c),device(d) {}

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
	
	irr::SKeyMap control_scheme[] = {
		{irr::EKA_MOVE_FORWARD,irr::KEY_KEY_W},
		{irr::EKA_MOVE_BACKWARD,irr::KEY_KEY_S},
		{irr::EKA_STRAFE_LEFT,irr::KEY_KEY_A},
		{irr::EKA_STRAFE_RIGHT,irr::KEY_KEY_D},
		{irr::EKA_JUMP_UP,irr::KEY_SPACE}
		
	};
	
	camera = scenemgr.addCameraSceneNodeFPS(
		0,
		100,
		0.5f,
		-1,
		control_scheme,
		5,
		false,
		5.0f
	);
	scene_nodes["camera"] = camera;
	camera->bindTargetAndRotation(true);
	camera->setPosition(irr::core::vector3df(player_pos.pos_x,player_pos.pos_y,player_pos.pos_z));
	
	device.getCursorControl()->setVisible(false);
	
	//! \todo rotate camera based on player's stored direction
	camera->setTarget(irr::core::vector3df(500.0f,0.0f,500.0f));
	
	terrain = scenemgr.addTerrainSceneNode(
		irr::io::path(heightmap.c_str()),
		0,
		-1,
		irr::core::vector3df(-5000.0f,-50.0f,-5000.0f),
		irr::core::vector3df(0.0f,0.0f,0.0f),
		irr::core::vector3df(500.0f,50.0f,500.0f),
		irr::video::SColor(255,255,255,255),
		5,
		irr::scene::ETPS_17,
		8
	);
	scene_nodes["terrain"] = terrain;
	
	terrain->setMaterialTexture(
		0,
		scenemgr.getVideoDriver()->getTexture(texture.c_str())
	);
	terrain->setMaterialFlag(irr::video::EMF_LIGHTING,true);
	
	terrain->scaleTexture(1.0f);
	
	world_selector = scenemgr.createMetaTriangleSelector();
	
	irr::scene::ISceneNode* sky = scenemgr.addSkyDomeSceneNode(
		scenemgr.getVideoDriver()->getTexture("sky/dome.tga"),
		16,
		16,
		1.0f,
		1.0f
	);
	scene_nodes["sky"] = sky;
	
	//! \todo load buildings from an XML file of assets
	buildings = {
			{
				"hut",
				Building(
					scenemgr,
					"buildings/hut.xml"
				)
			}
		};
	
	
	// collision detection code 
	{
		colmgr = scenemgr.getSceneCollisionManager();
		irr::scene::ITriangleSelector* terrain_selector = scenemgr.createTerrainTriangleSelector(terrain);
		world_selector->addTriangleSelector(terrain_selector);
		// make buildings collidable
		for(std::map<std::string,Building>::iterator i = buildings.begin(); i != buildings.end(); ++i)
		{
			world_selector->addTriangleSelector(i->second.get_mesh_node()->getTriangleSelector());
		}
		
		collision_material.setTexture(0,0);
		collision_material.Lighting = false;
		collision_material.Wireframe = true;
		
		irr::scene::ISceneNodeAnimator* gravity_animator = scenemgr.createCollisionResponseAnimator(
			world_selector,
			camera,
			irr::core::vector3df(30,50,30),
			irr::core::vector3df(0,-10,0),
			irr::core::vector3df(0,30,0)
		);
		camera->addAnimator(gravity_animator);
		
	}
	
	sun = scenemgr.addLightSceneNode(
		camera,
		irr::core::vector3df(0,0,0),
		irr::video::SColor(255,255,255,255),
		5000.0f,
		-1
	);
	sun->setLightType(irr::video::ELT_POINT);
	sun->setRadius(180.0f);
	sun->enableCastShadow(true);
	
	#if __linux
		// set building checker thread running
		bchck_thread = std::thread(&MapScene::run_bchk_thread,this);
	#endif
	
	ready = true;
}

#if __linux

	void MapScene::run_bchk_thread(MapScene* ms)
	{
		ms->set_inotify_file(inotify_init());
		ms->do_inotify_setup();
	}

	void MapScene::do_inotify_setup(void)
	{
		std::map<int,std::string> watches;
		int index = 0;
		for(std::map<std::string,Building>::iterator i = buildings.begin(); i != buildings.end(); ++i)
		{
			index = inotify_add_watch(inotify_file,i->second.get_asset_file().c_str(),IN_MODIFY);
			watches[index] = i->first;
		}
		while(engine.running) // otherwise this thread won't get cleaned up cleanly
		{
			#define EVENT_BUFFER_SIZE (sizeof(inotify_event))
			inotify_event event_buffer;
			read(inotify_file,&event_buffer,sizeof(inotify_event));
			reload_building(watches[event_buffer.wd]);
			if(event_buffer.mask & IN_IGNORED)
			{
				std::string building_name = watches.find(event_buffer.wd)->second;
				std::string filepath = buildings.find(building_name)->second.get_asset_file().c_str();
				watches[inotify_add_watch(inotify_file,filepath.c_str(),IN_MODIFY)] = building_name;
			}
		}
	}

	void MapScene::reload_building(std::string which)
	{
		buildings.find(which)->second.reload();
	}

	void MapScene::set_inotify_file(int fd)
	{
		inotify_file = fd;
	}

#endif

void MapScene::render(void)
{
	irr::core::line3d<irr::f32> ray;
	ray.start = camera->getPosition();
	ray.end = ray.start - (irr::core::vector3df(0.0f,500.0f,0.0f));
	irr::core::vector3df intersection;
	
	const irr::scene::ISceneNode* target_node = 0;
	colmgr->getCollisionPoint(
		ray,
		world_selector,
		intersection,
		hit_triangle,
		target_node
	);
	
	//sun->setPosition(intersection + irr::core::vector3df(0.0f,1500.0f,0.0f));
	
	device.getVideoDriver()->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());
    device.getVideoDriver()->setMaterial(collision_material);
	device.getVideoDriver()->draw3DTriangle(hit_triangle, irr::video::SColor(0,255,0,255));
	
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
