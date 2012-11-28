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
		char token; // randomised number unique to each connection
		std::string auth; // retrieved from the database when the username is sent
		sf::TcpSocket* sock;
		bool bad_user;
		bool authorised;
	};
	class SFMLNetwork : public Network
	{
	public:
		SFMLNetwork(Logger& log,Database& db);
		~SFMLNetwork();
		bool listen(const unsigned short port);
		bool send(const std::string& msg);
		bool receive(std::string& result);
		bool login(const std::string& username, const std::string& password);
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
		unsigned int expected_size;
		bool command_mode;
		NetworkCommand last_command;
	};
};

#endif
