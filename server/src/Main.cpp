#include "Configuration.hpp"
#include "Options.hpp"
#include "Server.hpp"
#include "SFMLNetwork.hpp"
#include <iostream>
#include <string>
#include <thread>

using namespace Citizens;

int main(int argc, char* argv[])
{
	Configuration config;
	Logger logger;
	Database db;
	Network* net = new SFMLNetwork(logger,db);
	Options opt(argc,argv);
	if(opt.print_help)
	{
		Options::help();
		return 1;
	}
	Server serv(opt,*net,logger,db);
	
	config = serv.init();
	
	if(!serv)
	{
		logger.log("Error starting server: "+serv.get_error(),ERROR);
		if(!config.non_fatal_errors)
			return serv.get_errnum();
	}
	
	if(config.auto_restart)
	{
		while(serv) // while server is valid, ie no error has occurred
		{
			logger.log("Starting server thread in auto-restart mode",MESSAGE);
			std::thread server_thread(&Server::run,&serv);
			server_thread.join();
			if(serv)
				logger.log("Restarting server",MESSAGE);
		}
	}
	else
	{
		logger.log("Starting server thread in no-restart mode",MESSAGE);
		std::thread server_thread(&Server::run,&serv);
		server_thread.join();
	}
	
	return 0;
}
