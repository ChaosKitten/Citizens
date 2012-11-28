#include "Server.hpp"
#include "Command.hpp"
#include "Database.hpp"
#include "SFMLNetwork.hpp"
#include <sstream>
#include <fstream>
#include <thread>

// non-blocking IO stuffs
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

using namespace Citizens;

Server::Server(Options& o,Network& n,Logger& l,Database& d) : opt(o), logger(l), net(n), db(d)
{
	errnum = NONE;
	running = false;
	prompt = ":> ";
}

Configuration Server::init()
{
	if(opt.config != "")
	{
		logger.log("Reading configuration from " + opt.config,DEBUG);
		config.read_from_file(opt.config);
		if(!config.valid()) 
		{
			logger.log("Invalid line in config file: "+config.invalid_line,ERROR);
			errnum = BAD_CONFIG;
		}
	}
	else
	{
		logger.log("Starting server with default (hardcoded) configuration settings",WARN);
		logger.log("This is not recommended, please use the -c or --config= options to specify a configuration file.",WARN);
	}
	
	db = Database(config.database);
	if(!db.connect())
	{
		logger.log("Database error: " + db.get_error(),ERROR);
		errnum = BAD_DATABASE;
	}
	
	if(!net.listen())
	{
		logger.log("Network error: " + net.get_error(),ERROR);
		errnum = BAD_NETWORK;
	}

	return config;
}

void Server::run(void* s)
{
	static_cast<Server*>(s)->start();
}

void Server::start(void)
{
	logger.log("Server started",MESSAGE);
	if(errnum == NONE) 
	{
		running = true;
		loop();
	}
	else
	{
		logger.log("Server could not start due to a pre-existing error",ERROR);
	}
}

void Server::stop(void)
{
	logger.log("Shutting down (Server::stop())",DEBUG);
	running = false;
}

void Server::loop(void)
{
	std::ifstream infile;
	if(opt.batch_mode)
	{
		logger.log("Starting in batch mode",MESSAGE);
		logger.log("Opening batch file "+opt.batch_file,MESSAGE);
		infile.open(opt.batch_file);
	}
	else
	{
		logger.log("Starting in interactive mode",MESSAGE);
	}
	logger.log("Entering main loop",DEBUG);
	bool prompt_needed = true;
	while(running)
	{
		// without this line, turning non-fatal errors off after
		// an error occurred would crash the server
		// which is undesirable behaviour, I believe
		errnum = NONE;
		std::string input;
		if(!opt.batch_mode)
		{
			if(prompt_needed)
			{
				std::cout << prompt;
				prompt_needed = false;
			}
			std::cout.flush();
			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 0;
			fd_set readfds,master;
			FD_ZERO(&readfds);
			FD_ZERO(&master);
			FD_SET(0,&readfds);
			FD_SET(0,&master);
			select(1,&readfds,NULL,NULL,&tv);
			if(FD_ISSET(0,&readfds))
			{
				std::getline(std::cin,input);
				prompt_needed = true;
			}
		}
		else
		{
			std::getline(infile,input);
		}
		Command c;
		c.parse(input);
		if(config.echo_commands)
			logger.log(c.to_string(),MESSAGE);
		switch(c)
		{
			case KILL:
			{
				errnum = KILLED;
				logger.log("Server killed",ERROR);
				break;
			}
			case QUIT:
			{
				running = false;
				logger.log("Shutting down",MESSAGE);
				break;
			}
			case INVALID:
			{
				logger.log("Invalid command",WARN);
				break;
			}
			case RELOAD:
			{
				logger.log("Reloading server configuration from " + opt.config,MESSAGE);
				config.read_from_file(opt.config);
				break;
			}
			case FORCE_ERROR:
			{
				std::stringstream error_builder;
				error_builder << "User-defined error: ";
				while(c.args.size() != 0)
				{
					error_builder << c.args.top() << " ";
					c.args.pop();
				}
				logger.log(error_builder.str(),ERROR);
				errnum = FORCED_ERROR;
				break;
			}
			case SET_CONFIG:
			{
				if(c.args.size() < 2)
				{
					logger.log("Too few arguments to command 'set'",WARN);
				}
				else
				{
					if(c.args.size() > 2)
					{
						logger.log("Commands after the first two to 'set' are ignored.",WARN);
					}
					std::string var_name = c.args.top();
					c.args.pop();
					std::string new_value = c.args.top();
					logger.log(var_name + " = " + new_value,MESSAGE);
					config.set(var_name,new_value);
				}
				break;
			}
			case HELP:
			{
				logger.log("Valid commands:",MESSAGE);
				std::map<std::string,std::string> ch = Command::command_help();
				for(std::map<std::string,std::string>::iterator i = ch.begin();i!=ch.end();i++)
				{
					std::stringstream help_text;
					help_text << i->first << ": " << i->second;
					logger.log(help_text.str(),MESSAGE);
				}
				break;
			}
			case LIST_USERS:
			{
				std::map<std::string,bool> users;
				bool success = db.get_userlist(users);
				if(!success)
				{
					logger.log("Database error: " + db.get_error(),WARN);
				}
				else
				{
					logger.log("Citizens:",MESSAGE);
					std::vector<std::string> online_users;
					std::vector<std::string> offline_users;
					for(std::map<std::string,bool>::iterator i = users.begin();i != users.end(); i++)
					{
						if(i->second) online_users.push_back(i->first);
						else offline_users.push_back(i->first);
					}
					logger.log("Online:",MESSAGE);
					for(unsigned int i = 0;i<online_users.size();i++)
					{
						logger.log("\t" + online_users[i],MESSAGE);
					}
					if(c.args.size() == 0 || c.args.top() != "online")
					{
						logger.log("Offline:",MESSAGE);
						for(unsigned int i = 0;i<offline_users.size();i++)
						{
							logger.log("\t" + offline_users[i],MESSAGE);
						}
					} // if(c.args[0] != "online")
				} // else, if(!success), success = db.get_userlist(users);
				break;
			} // case LIST_USERS:
			case LIST_ACCOUNTS:
			{
				std::vector<std::string> accounts;
				bool success = db.get_accountlist(accounts);
				if(!success)
				{
					logger.log("Database error: " + db.get_error(),WARN);
				}
				else
				{
					logger.log("Accounts:",MESSAGE);
					for(std::vector<std::string>::iterator i = accounts.begin();i != accounts.end(); ++i)
					{
						logger.log("\t" + *i,MESSAGE);
					}
				}
				break;
			}
			case ADD_ACCOUNT:
			{
				std::string username = c.args.top();
				c.args.pop();
				std::string password = c.args.top();
				bool success = db.add_account(username,password);
				if(success)
				{
					logger.log("User " + username + " added successfully!",MESSAGE);
				}
				else
				{
					logger.log("Database error: " + db.get_error(),WARN);
				}
				break;
			}
			case EMPTY_COMMAND:
			{
				if(!opt.batch_mode)
				{
					//logger.log("You pressed enter without typing in a command, dummy",DEBUG);
				}
				break;
			}
			case ECHO:
			{
				if(config.immediate_echo)
				{
					while(c.args.size() != 0)
					{
						std::cout << c.args.top() << " ";
						c.args.pop();
					}
					std::cout << std::endl;
				}
				else
				{
					while(c.args.size() != 0)
					{
						logger.log(c.args.top(),MESSAGE);
						c.args.pop();
					}
				}
			}
		} // switch(c)
		
		net.tick();
		
		// carefully break the loop based on whether errors are supposed to be
		// fatal or not
		if((errnum == KILLED) || ((errnum != NONE) && !config.non_fatal_errors))
			break;
	}
	if(infile.is_open()) infile.close();
}

Server::operator bool(void)
{
	return (errnum == NONE);
}

int Server::get_errnum(void)
{
	return errnum;
}

std::string Server::get_error(void)
{
	std::string error;
	switch(errnum)
	{
		case NONE:
			error = "No error.";
			break;
		case BAD_CONFIG:
			error = "Bad config.";
			break;
		case BAD_DATABASE:
			error = "Database error, check error logger.log.";
			break;
		default:
			error = "Unknown error.";
			break;
	}
	return error;
}
