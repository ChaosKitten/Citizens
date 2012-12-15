#ifndef TESTNETWORK_HPP
#define TESTNETWORK_HPP

#include "Network.hpp"

namespace Citizens
{
	class TestNetwork : public Network
	{
	public:
		bool connect(const std::string& ip);
		bool send(NetworkCommand c,char payload_length,const std::string& payload);
		bool login(const std::string& username, const std::string& password);
		std::string get_error(void);
		bool is_disconnected(void);
		void get_pos(Coordinate& c);
		irr::io::IXMLReader* get_XML(const std::string& resource_name);
	};
	
};

#endif
