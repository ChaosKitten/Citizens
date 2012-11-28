#include "Logger.hpp"
#include <iostream>

using namespace Citizens;

void Logger::log(const std::string& message,debug_level threshold)
{
	switch(threshold)
	{
		case WARN:
			std::cout << "[\033[1;33mWARNING\033[0m] " << message << std::endl;
			break;
		case ERROR:
			std::cerr << "[\033[1;31mERROR\033[0m] " << message << std::endl;
			break;
		case MESSAGE:
			std::cout << "[\033[1;34mINFO\033[0m] " << message << std::endl;
			break;
		case DEBUG:
			std::cout << "[DEBUG] " << message << std::endl;
			break;
		case NET:
			std::cout << "[NET] " << message << std::endl;
			break;
	}
}
