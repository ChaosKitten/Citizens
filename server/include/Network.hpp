#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <string>

namespace Citizens
{
	class Network
	{
	public:
		virtual bool listen(const unsigned short port = 3177) = 0;
		virtual bool send(const std::string& msg) = 0;
		virtual bool receive(std::string& result) = 0;
		virtual bool login(const std::string& username, const std::string& password) = 0;
		virtual bool tick(void) = 0;
		virtual std::string get_error(void) = 0;
	};
};

#endif
