#include "Command.hpp"
#include <string>
#include <iostream>
#include <sstream>

using namespace Citizens;

void Command::parse(const std::string& raw)
{
	// sanitize input:
	if(raw=="") {
		value = EMPTY_COMMAND;
		return;
	}
	std::stringstream tokens(raw);
	std::string command;
	std::string arg;
	std::stack<std::string> args_tmp;
	std::getline(tokens,command,' ');
	while(std::getline(tokens,arg,' '))
	{
		args_tmp.push(arg);
	}
	// reverse the list of arguments so that the 'top' of the stack
	// becomes the left-most argument and the 'bottom' is the right-most
	// argument
	while(args_tmp.size() != 0)
	{
		args.push(args_tmp.top());
		args_tmp.pop();
	}
	value = INVALID; // assume invalid unless proven otherwise
	if(command == "quit" || command == "exit" || command == "q") value = QUIT;
	if(command == "help" || command == "h") value = HELP;
	if(command == "reload" || command == "r") value = RELOAD;
	if(command == "kill" || command == "k") value = KILL;
	if(command == "force_error" || command == "fe") value = FORCE_ERROR;
	if(command == "set" || command == "s") value = SET_CONFIG;
	if(command == "list" || command == "l")
	{
		if(args.size() == 0)
			value = INVALID;
		else if(args.top() == "users")
			value = LIST_USERS;
		else if(args.top() == "accounts")
			value = LIST_ACCOUNTS;
		
		args.pop();
	}
	if(command == "add" || command == "a") value = ADD_ACCOUNT;
	if(command == "echo" || command == "e") value = ECHO;
	//std::cout << raw << " interpreted as " << to_string() << std::endl;
}
 
Command::operator internal_command_enum() const
{
	return value;
}

std::map<std::string,std::string> Command::command_help(void)
{
	std::map<std::string,std::string> help;
	help["help"] = "displays this help";
	help["quit"] = "shuts down the server";
	help["exit"] = "alias of quit";
	help["reload"] = "reloads the configuration file without restarting the server";
	help["kill"] = "shut down the server, bypassing the auto-restart behaviour";
	help["list users"] = "if the argument \"online\" is passed, lists only online users";
	help["list accounts"] = "lists all available accounts (server admin command)";
	help["add"] = "adds a new account";
	help["set"] = "sets a configuration variable at run-time";
	help["echo"] = "print a string of text";
	//! \todo set up development vs release build systems and include this in the development build
	// help["force_error"] = "forces the error to emit an error and set the error number.  development purposes only";
	return help;
}

std::string Command::to_string(void)
{
	std::stringstream builder;
	switch(value)
	{
		case QUIT:
			builder << "Command: quit ";
			break;
		case INVALID:
			builder << "Invalid command ";
			break;
		case KILL:
			builder << "Command: kill ";
			break;
		case FORCE_ERROR:
			builder << "Command: force_error ";
			break;
		case RELOAD:
			builder << "Command: reload ";
			break;
		case HELP:
			builder << "Command: help ";
			break;
		case SET_CONFIG:
			builder << "Command: set ";
			break;
		case LIST_USERS:
			builder << "Command: list ";
			break;
		case ADD_ACCOUNT:
			builder << "Command: add ";
			break;
	}
	if(!args.empty())
	{
		unsigned int i = 0;
		std::stack<std::string> tmp = args;;
		while(tmp.size() != 0)
		{
			builder << tmp.top() << " ";
			tmp.pop();
			i++;
		}
	}
	return builder.str();
}
