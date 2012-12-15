#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <string>

namespace Citizens
{
	class Network
	{
	public:
		virtual bool listen(const unsigned short port = 3177) = 0;
		virtual bool tick(void) = 0;
		virtual std::string get_error(void) = 0;
	};
};

#endif
