#ifndef MAPSCENE_HPP
#define MAPSCENE_HPP

#include "IScene.hpp"
#include "Coordinate.hpp"
#include "Building.hpp"
#include "GraphicsEngine.hpp"
#include <map>
#include <thread>

namespace Citizens
{	
	class MapScene : public IScene
	{
	public:
		MapScene(GraphicsEngine& ge,const Coordinate& c, irr::scene::ISceneManager& sm,irr::IrrlichtDevice& d);
		void init(const Configuration& config);
		void unload(void);
		bool is_setup(void);
		void render(void);
	private:
	
		#if __linux
			static void run_bchk_thread(MapScene* ms);
			void reload_building(std::string which);
			void set_inotify_file(int fd);
			void do_inotify_setup(void);
			
			int inotify_file;
		#endif
		
		std::thread bchck_thread;
		std::map<std::string,Building> buildings;
		irr::scene::ISceneManager& scenemgr;
		GraphicsEngine& engine;
		bool ready;
		std::map<std::string,irr::scene::ISceneNode*> scene_nodes;
		typedef std::map<std::string,irr::scene::ISceneNode*>::iterator scene_node_it;
		const Coordinate& player_pos;
		irr::IrrlichtDevice& device;
		irr::core::triangle3df hit_triangle;
		irr::video::SMaterial collision_material;
		irr::scene::ITerrainSceneNode* terrain;
		irr::scene::ICameraSceneNode* camera;
		irr::scene::ISceneCollisionManager* colmgr;
		irr::scene::IMetaTriangleSelector* world_selector;
		irr::scene::ILightSceneNode* sun;
	};

};


#endif

