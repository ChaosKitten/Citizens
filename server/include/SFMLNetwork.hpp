#ifndef SFMLNETWORK_HPP
#define SFMLNETWORK_HPP

#include "Network.hpp"
#include "Logger.hpp"
#include "NetworkCommand.hpp"
#include "Database.hpp"
#include <SFML/Network.hpp>

namespace Citizens
{
	struct ClientData
	{
		std::string username;
		unsigned char token; // randomised number unique to each connection
		std::string auth; // retrieved from the database when the username is sent
		sf::TcpSocket* socket;
		bool bad_user;
		bool authorised;
		unsigned char* packet;
		unsigned int packet_size;
		unsigned int packet_expected;
		bool disconnected;
		bool online;
	};
	class SFMLNetwork : public Network
	{
	public:
		SFMLNetwork(Logger& log,Database& db);
		~SFMLNetwork();
		bool listen(const unsigned short port);
		std::string get_error(void);
		bool tick(void);
		void stopThread(void);
	private:
		std::string error;
		sf::TcpListener socket;
		std::vector<ClientData> active_connections;
		Logger& logger;
		Database& database;
		bool running;
		bool command_mode;
		NetworkCommand last_command;
		bool send_byte(ClientData client,const unsigned char& byte,bool& sent);
		bool get_byte(ClientData& client,unsigned char& byte);
		bool send_all(ClientData client,const unsigned char* data, const unsigned int size);
	};
};

#endif
