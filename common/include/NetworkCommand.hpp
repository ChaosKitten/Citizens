#ifndef NetworkCommand_HPP
#define NetworkCommand_HPP

#include <string>
#include <map>

namespace Citizens
{
	enum eNetworkCommand
	{
		ENC_LOGIN,
		ENC_AUTHORISATION,
		ENC_NO_COMMAND, // this command is used for transmitting 'anonymous' data to and fro, its usefulness is yet to be proven, however.
		ENC_INVALID
	};
	class NetworkCommand
	{
	public:
		NetworkCommand(const eNetworkCommand& e);
		NetworkCommand(const std::string& v);
		NetworkCommand() : value(ENC_INVALID) {}
		operator std::string();
		static const int length = 5;
		bool operator ==(const eNetworkCommand& e) const { return (e == value); }
		operator eNetworkCommand() { return value; }
		std::string str();
	private:
		eNetworkCommand value;
		static const std::map<std::string,eNetworkCommand> commands;
	};
};

#endif
