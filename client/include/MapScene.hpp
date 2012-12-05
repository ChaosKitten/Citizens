#ifndef MAPSCENE_HPP
#define MAPSCENE_HPP

#include "IScene.hpp"
#include "Coordinate.hpp"
#include <map>

namespace Citizens
{

	class MapScene : public IScene
	{
	public:
		MapScene(const Coordinate& c, irr::scene::ISceneManager& sm);
		void init(const Configuration& config);
		void unload(void);
		bool is_setup(void);
	private:
		irr::scene::ISceneManager& scenemgr;
		bool ready;
		std::map<std::string,irr::scene::ISceneNode*> scene_nodes;
		typedef std::map<std::string,irr::scene::ISceneNode*>::iterator scene_node_it;
		const Coordinate& player_pos;
	};

};


#endif

