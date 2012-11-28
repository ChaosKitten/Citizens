#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
#include <iostream>
#include <map>

namespace Citizens
{
	struct db_config
	{
		std::string host;
		unsigned int port;
		std::string user;
		std::string password;
		std::string schema;
	};
	class Configuration
	{
	public:
		Configuration();
		~Configuration();
		bool auto_restart;
		bool non_fatal_errors;
		bool echo_commands;
		
		bool immediate_echo;
		db_config database;
		void read_from_file(const std::string& filename);
		bool valid(void);
		std::string invalid_line;
		void set(std::string property,std::string to);
	private:
		void parse_line(const std::string& line);
		bool is_valid;
	};
};


#endif
