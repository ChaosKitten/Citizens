#include "NetworkCommand.hpp"

using namespace Citizens;

const std::map<std::string,eNetworkCommand> NetworkCommand::commands = {
	{"LOGIN",ENC_LOGIN},
	{"AUTHR",ENC_AUTHORISATION},
	{"BLANK",ENC_NO_COMMAND},
	// IMPORTANT: DO NOT ADD "INVALID" TO THIS STRUCTURE
};

NetworkCommand::NetworkCommand(const eNetworkCommand& e):value(e){}

NetworkCommand::NetworkCommand(const std::string& v)
{
	std::map<std::string,eNetworkCommand>::const_iterator c = commands.find(v);
	if(c!=commands.end())
		value = c->second;
	else
		value = ENC_INVALID; // specific from 'NO_COMMAND' for a reason
}

std::string NetworkCommand::str()
{
	for(std::map<std::string,eNetworkCommand>::const_iterator c = commands.begin(); c!=commands.end(); ++c)
	{
		if(c->second == value) return c->first; // break out of the loop immediately
	}
	return "INVLD";
}

NetworkCommand::operator std::string()
{
	return this->str();
}
