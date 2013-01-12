#ifndef CCLIENT_ENGINE_LINUX_HPP
#define CCLIENT_ENGINE_LINUX_HPP

#include "IScene.hpp"

namespace citi
{
	class CClientEngineLinux : public IClientEngine
	{
	public:
		void run(void);
		virtual IScene* create_menu_scene(const std::string& menu_file);
		virtual IScene* create_overview_scene(ICityMap* map);
	private:
		IScene* current_scene;
	};
};

#endif
