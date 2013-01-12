#include "IServerEngine.hpp"
#include "CException.hpp"

int main(int argc, char* argv[])
{
	citi::COptions opt(argc,argv);
	int rv = 0;
	citi::IServerEngine* engine = citi::create_server_engine(opt);
	try {
		engine->run();
	} catch (citi::CException e) {
		e.print_error();
		rv = e;
	}
	delete engine;
	return rv;
}
