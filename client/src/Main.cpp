#include "TestNetwork.hpp"
#include "GraphicsEngine.hpp"

#include <iostream>

using namespace Citizens;

#define SERVER_IP "127.0.0.1"

int main(void)
{
	Protocol p;
	Network* net = new TestNetwork();
	GraphicsEngine ge(net);
	ge.getConfig().setProperty("xres",800);
	ge.getConfig().setProperty("yres",600);
	
	std::string position;
	if(!net->connect(SERVER_IP))
	{
		std::cerr << "Error: could not connect to" SERVER_IP " : " << net->get_error() << std::endl;
		delete net;
		return -1;
	}
	
	ge.init();
	
	// offline mode needs a lot of polish, but at the moment it stops an infinite reconnection attempt loop
	bool offline_mode = false;
	while(ge.running)
	{
		if(!offline_mode)
		{
			if(net->is_disconnected()) 
			{
				unsigned int i = 0;
				#define RECONNECT_ATTEMPTS 3
				while(i < RECONNECT_ATTEMPTS && net->is_disconnected())
				{
					if(!net->connect(SERVER_IP))
					{
						std::cerr << "Attempt #" << i + 1 << " failed: " << net->get_error() << std::endl;
					}
					i++;
					// forces the program to wait between attempts, which is a bad idea but shh, I'm trying something here...
					sleep(1);
				}
				if(i >= RECONNECT_ATTEMPTS)
				{
					std::cerr << "Could not reconnect to the network, continuing in offline mode." << std::endl;
					offline_mode = true;
				}
			}
		}
		ge.render();
	}
	
	
	delete net;
	return 0;
}
