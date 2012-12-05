#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "Protocol.hpp"
#include "NetworkCommand.hpp"
#include "Coordinate.hpp"
#include <string>

#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <irrlicht.h>
#pragma GCC diagnostic pop

namespace Citizens
{
	class Network
	{
	public:
		virtual bool connect(const std::string& ip) = 0;
		virtual bool send(NetworkCommand c,char payload_length,const std::string& payload) = 0;
		virtual bool login(const std::string& username, const std::string& password) = 0;
		virtual std::string get_error(void) = 0;
		virtual bool is_disconnected(void) = 0;
		virtual void get_pos(Coordinate& c) = 0;
	};
	
};

#endif
