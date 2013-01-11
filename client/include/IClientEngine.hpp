#ifndef ICLIENT_ENGINE_HPP
#define ICLIENT_ENGINE_HPP

// first the class definition (most important part of the file)
namespace citi
{
	class IClientEngine
	{
	public:
		virtual void run(void) = 0;
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
