#ifndef COVERVIEW_SCENE_HPP
#define COVERVIEW_SCENE_HPP

#include "IScene.hpp"
#include "ICityMap.hpp"

namespace citi
{
	class COverviewScene : public IScene
	{
	public:
		COverviewScene(citi::ICityMap* m) : map(m) {}
		void render(void);
	private:
		ICityMap* map;
	};
};

#endif
