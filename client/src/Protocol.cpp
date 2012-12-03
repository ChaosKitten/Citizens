#include "Protocol.hpp"

using namespace Citizens;

std::map<std::string,command> Protocol::valid_commands = {
		{"getpos",GETPOS}
	};


command Protocol::parse(const std::string& cmd) const
{
	if(valid_commands.count(cmd) > 0)
		return valid_commands[cmd];
	else
		return INVALID;
}
