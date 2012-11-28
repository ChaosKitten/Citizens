#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>

namespace Citizens
{
	enum debug_level
	{
		ERROR,
		WARN,
		DEBUG,
		MESSAGE,
		NET
	};
	class Logger
	{
	public:
		void log(const std::string& message,debug_level type);
		
	};
};

#endif
