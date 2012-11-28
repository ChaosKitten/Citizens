#ifndef LOGINSCENE_HPP
#define LOGINSCENE_HPP

#include "IScene.hpp"

namespace Citizens
{

	class LoginScene : IScene
	{
	public:
		LoginScene(irr::scene::ISceneManager& sm);
		void init(const Configuration& config);
		irr::scene::ISceneManager& operator()();
	private:
		irr::scene::ISceneManager& scenemgr;
	};

};


#endif
