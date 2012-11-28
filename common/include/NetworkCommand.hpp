#ifndef NetworkCommand_HPP
#define NetworkCommand_HPP

#include <string>

namespace Citizens
{
	enum eNetworkCommand
	{
		LOGIN,
		AUTHORISATION,
		NO_COMMAND
	};
	class NetworkCommand
	{
	public:
		NetworkCommand(eNetworkCommand e);
		operator std::string();
		static const int length;
		bool operator ==(eNetworkCommand e) { return (e == value); }
		operator eNetworkCommand() { return value; }
	private:
		eNetworkCommand value;
	};
};

#endif
