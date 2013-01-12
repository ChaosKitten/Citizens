#ifndef ISERVER_ENGINE_HPP
#warning "CServerEngine.hpp should not be included directly - errors are likely to occur after seeing this warning"
#else

#ifndef CSERVER_ENGINE_HPP
#define CSERVER_ENGINE_HPP

#include "COptions.hpp"

namespace citi
{
	class CServerEngine : public IServerEngine
	{
	public:
		CServerEngine(COptions opt) : options(opt) {}
		void run(void);
	private:
		COptions options;
	};
};

#endif
#endif
