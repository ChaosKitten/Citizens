#include "IClientEngine.hpp"
#include "CClientEngineLinux.hpp"
#include "CException.hpp"
#include "COverviewScene.hpp"

#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <irrlicht.h>
#pragma GCC diagnostic pop

void citi::CClientEngineLinux::run(void)
{
	current_scene = this->create_menu_scene();
}

citi::IScene* citi::CClientEngineLinux::create_menu_scene(citi::IMenuPage* page)
{
	return NULL;
}

citi::IScene* citi::CClientEngineLinux::create_overview_scene(citi::ICityMap* map)
{
	return new COverviewScene(map);
}

citi::CClientEngineLinux::~CClientEngineLinux()
{
}
