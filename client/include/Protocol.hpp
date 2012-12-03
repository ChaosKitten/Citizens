#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <map>
#include <string>

namespace Citizens
{
	/**
	 * \brief Deprecated enum for what is now the NetworkCommand class
	 * \deprecated in favour of NetworkCommand
	 * \todo delete this and all references to it
	 */
	enum command
	{
		GETPOS,
		INVALID
	};
	/**
	 * \brief Protocol class containing 
	 */
	class Protocol
	{
	public:
	//! \warning Protocol::parse is redundant and scheduled for deletion
	//! \note use NetworkCommand::NetworkCommand(const std::string&) instead
		command parse(const std::string& cmd) const;
		static std::map<std::string,command> valid_commands;
		static const unsigned short int port = 3177;
	private:
	};
	
	
};

#endif
