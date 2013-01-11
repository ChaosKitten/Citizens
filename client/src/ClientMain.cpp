#include "IClientEngine.hpp"
#include "CException.hpp"

int main(void)
{
	int rv = 0;
	citi::IClientEngine* engine = citi::create_client_engine();
	try {
		engine->run();
	} catch (citi::CException e) {
		e.print_error();
		rv = e;
	}
	return rv;
}
