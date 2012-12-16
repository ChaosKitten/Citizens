#include "CachedXML.hpp"

using namespace Citizens;

irr::io::IXMLReader* CachedXML::fetch()
{
	// bypass both the disk and the network if we have it in memory
	if(data == NULL) if(!fetch_disk()) fetch_net();
	return data;
}

void CachedXML::fetch_net()
{
	network.get_XML(resource_name);
}

bool CachedXML::fetch_disk()
{
	bool hit = false;
	irr::core::stringw resource_path = L"cache/";
	resource_path.append(resource_name.c_str());
	if(file_system.existFile(resource_path))
	{
		data = file_system.createXMLReader(resource_path);
		hit = true;
	}
	return hit;
}
