#ifndef ICLIENT_ENGINE_HPP
#define ICLIENT_ENGINE_HPP

#include "IMenuPage.hpp"
#include "CLoginMenuPage.hpp"
#include <string>
// first the class definition (most important part of the file)
namespace citi
{
	// forward-declare these so we don't bump into each other
	class IScene; 
	class ICityMap;
	class IClientEngine
	{
	public:
		virtual void run(void) = 0;
		virtual IScene* create_menu_scene(IMenuPage* page = new CLoginMenuPage()) = 0;
		virtual IScene* create_overview_scene(ICityMap* map) = 0;
	};
};

// then all the inline functions that go with the IClientEngine territory
#include "CClientEngine.hpp"
namespace citi
{
	inline IClientEngine* create_client_engine(void)
	{
		#if __linux
		return new CClientEngineLinux();
		#elif __apple__
		return new CClientEngineMacOSX();
		#elif __WIN32 || __WIN64
		return new CClientEngineWindows();
		#else
		return new CClientEngineGeneric();
		#endif
	}
};

#endif
