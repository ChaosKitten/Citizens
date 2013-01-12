#ifndef ISERVER_ENGINE_HPP
#define ISERVER_ENGINE_HPP

namespace citi
{
	class IServerEngine
	{
	public:
		virtual void run(void) = 0;
	};
};
#include "CServerEngine.hpp"
#include "COptions.hpp"
namespace citi
{
	inline IServerEngine* create_server_engine(COptions opt)
	{
		return new CServerEngine(opt);
	}
};


#endif
