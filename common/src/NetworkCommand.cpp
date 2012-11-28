#include "NetworkCommand.hpp"

using namespace Citizens;

const int NetworkCommand::length = 5;

NetworkCommand::NetworkCommand(eNetworkCommand e):value(e){}

NetworkCommand::operator std::string()
{
	std::string rv;
	switch(value)
	{
		case LOGIN:
			rv = "LOGIN";
			break;
		case AUTHORISATION:
			rv = "AUTHR";
			break;
		case NO_COMMAND:
			rv = "BLANK";
			break;
	}
	return rv;
}
