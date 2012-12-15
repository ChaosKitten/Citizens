#include "Zone.hpp"

using namespace Citizens;

void Zone::unload(void)
{
	unsigned int i=0;
	for(i=0;i<9;i++)
	{
		if(tiles[i] != NULL)
		{
			tiles[i]->unload();
			delete tiles[i];
		}
	}
}
