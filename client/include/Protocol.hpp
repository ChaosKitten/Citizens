#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <map>
#include <string>

namespace Citizens
{
	enum command
	{
		GETPOS,
		INVALID
	};
	class Protocol
	{
	public:
		command parse(const std::string& cmd) const;
		static std::map<std::string,command> valid_commands;
		static const unsigned short int port = 3177;
	private:
	};
	
	
};

#endif
