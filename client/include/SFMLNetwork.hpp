#ifndef SFMLNETWORK_HPP
#define SFMLNETWORK_HPP

#include "Network.hpp"
#include <SFML/Network.hpp>

namespace Citizens
{
	class SFMLNetwork : public Network
	{
	public:
		SFMLNetwork(const Protocol& p);
		bool connect(const std::string& ip);
		bool send(NetworkCommand c,char payload_length,const std::string& payload);
		bool receive(unsigned char* buf,unsigned int size);
		bool login(const std::string& username, const std::string& password);
		std::string get_error(void);
		bool is_disconnected(void);
	private:
		const Protocol& protocol;
		std::string error;
		sf::TcpSocket socket;
		bool disconnected;
	};
};

#endif
