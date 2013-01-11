#include "CException.hpp"
#include <iostream>

citi::CException::CException(std::string r,bool f) : reason(r),fatal(f)
{
	
}

citi::CException::operator int(void)
{
	return status_number;
}

void citi::CException::print_error(void)
{
	std::cout << (fatal?"[ERR] ":"[WRN] ");
	std::cout << reason << std::endl;
}
