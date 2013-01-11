#ifndef CEXCEPTION_HPP
#define CEXCEPTION_HPP

#include <string>

namespace citi
{
	class CException
	{
	public:
		CException(std::string reason,bool fatal=false);
		void print_error(void);
		operator int(void);
	private:
		int status_number;
		std::string reason;
		bool fatal;
	};
};

#endif
