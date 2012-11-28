#include "Configuration.hpp"
#include "Trim.hpp"
#include "Server.hpp"
#include "Database.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

inline bool str2bool(const std::string& str)
{
	return
	(str=="yes" ||
	 str=="true" ||
	 str=="on");
}

using namespace Citizens;

Configuration::Configuration()
{
	auto_restart = false;
	is_valid = true;
	echo_commands = false;
	non_fatal_errors = false;
	
	database.host = "localhost";
	database.port = 3306;
	database.user = "chaoskitten";
	database.password = "password";
	database.schema = "citizens";
}

bool Configuration::valid(void)
{
	return is_valid;
}

void Configuration::parse_line(const std::string& in_line)
{
	std::string line(in_line);
	strip_comment(line);
	std::stringstream tokenizer(line);
	std::string option;
	std::string value;
	std::getline(tokenizer,option,'=');
	std::getline(tokenizer,value);
	if(trim(option) == "auto_restart")
	{
		auto_restart = str2bool(trim(value));
	}
	else if(trim(option) == "non_fatal_errors")
	{
		non_fatal_errors = str2bool(trim(value));
	}
	else if(trim(option) == "echo_commands")
	{
		echo_commands = str2bool(trim(value));
	}
	else if(trim(option) == "")
	{
		// skip blank lines
		return;
	}
	else if(trim(option) == "db_host")
	{
		database.host = trim(value);
	}
	else if(trim(option) == "db_user")
	{
		database.user = trim(value);
	}
	else if(trim(option) == "db_port")
	{
		std::stringstream port(trim(value));
		port >> database.port;
	}
	else if(trim(option) == "db_pass")
	{
		database.password = trim(value);
	}
	else if((trim(option) == "db_schema") || (trim(option) == "db_database"))
	{
		database.schema = trim(value);
	}
	else if(trim(option) == "immediate_echo")
	{
		immediate_echo = str2bool(trim(value));
	}
	else
	{
		is_valid = false;
		invalid_line = line;
	}
}

void Configuration::read_from_file(const std::string& filename)
{
	std::ifstream config(filename);
	std::string inbuf;
	while(std::getline(config,inbuf))
	{
		parse_line(inbuf);
	}
}

void Configuration::set(std::string property,std::string to)
{
	// defers to parse_line like a good little child
	std::stringstream builder;
	builder << property << "=" << to;
	parse_line(builder.str());
	if(!is_valid)
	{
		std::cerr << "[CODED_WARNING] Configuration value " << property << " probably doesn't exist" << std::endl;
		// since leaving the validity as false might break future code,
		// and the configuration hasn't exactly been "broken", I guess
		// we can reset it to true here
		is_valid = true;
	}
}

/**
 * \todo FIXME: potential memory leaks from *_log pointers
 */
Configuration::~Configuration()
{
	/* -- causes segfaults
	delete debug_log;
	if(*error_log != std::cerr) delete error_log;
	if(*message_log != std::cout) delete message_log;
	delete net_log;
	*/
}
