#ifndef CCLIENT_ENGINE_LINUX_HPP
#define CCLIENT_ENGINE_LINUX_HPP

#include "IScene.hpp"

namespace citi
{
	class CClientEngineLinux : public IClientEngine
	{
	public:
		~CClientEngineLinux();
		void run(void);
		virtual IScene* create_menu_scene(IMenuPage* page = new IMenuPage("login"));
		virtual IScene* create_overview_scene(ICityMap* map);
	private:
		IScene* current_scene;
	};
};

#endif
