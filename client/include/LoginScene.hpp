#ifndef LOGINSCENE_HPP
#define LOGINSCENE_HPP

#include "IScene.hpp"
#include <map>

namespace Citizens
{

	class LoginScene : public IScene
	{
	public:
		LoginScene(irr::scene::ISceneManager& sm);
		void init(const Configuration& config);
		void unload(void);
		bool is_setup(void);
		void render(void);
		irr::scene::ISceneManager& operator()();
	private:
		irr::scene::ISceneManager& scenemgr;
		bool ready;
		std::map<std::string,irr::scene::ISceneNode*> scene_nodes;
		typedef std::map<std::string,irr::scene::ISceneNode*>::iterator scene_node_it;
	};

};


#endif
