#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <stack>
#include <map>

namespace Citizens
{
	enum internal_command_enum
	{
		QUIT,
		INVALID,
		HELP,
		RELOAD,
		KILL,
		FORCE_ERROR,
		SET_CONFIG,
		LIST_USERS,
		ADD_ACCOUNT,
		LIST_ACCOUNTS,
		EMPTY_COMMAND,
		ECHO
	};
	class Command
	{
	public:
		void parse(const std::string& raw);
		operator internal_command_enum() const;
		std::string to_string(void);
		static std::map<std::string,std::string> command_help(void);
		std::stack<std::string> args;
	private:
		internal_command_enum value;
	};
};

#endif
