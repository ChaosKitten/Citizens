#ifndef CACHEDXML_HPP
#define CACHEDXML_HPP

#include "Network.hpp"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <irrlicht.h>
#pragma GCC diagnostic pop
#include <string>

namespace Citizens
{
	
	class CachedXML
	{
	public:
		CachedXML(const std::string& rn,irr::io::IFileSystem& fs,Network& n) : resource_name(rn),file_system(fs),network(n) { data = NULL; }
		~CachedXML() { delete data; }
		irr::io::IXMLReader* fetch(void);
	private:
		void fetch_net(void);
		//! \return true if cache hit, false if cache missed
		bool fetch_disk(void);
		void cache(void);
		const std::string& resource_name;
		irr::io::IFileSystem& file_system;
		Network& network;
		
		irr::io::IXMLReader* data;

		
	};
	
};


#endif
