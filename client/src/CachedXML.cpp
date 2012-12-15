#include "CachedXML.hpp"

using namespace Citizens;

irr::io::IXMLReader* CachedXML::fetch()
{
	if(!fetch_disk()) fetch_net();
}

void CachedXML::fetch_net()
{
	network.get_XML(resource_name);
}

bool CachedXML::fetch_disk()
{
	bool hit = false;
	if(file_system.fileExists("cache/"+resource_name))
	{
		data = file_system.createXMLReader("cache/"+resource_name);
		hit = true;
	}
	return hit;
}
