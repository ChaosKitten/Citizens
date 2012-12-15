#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <map>
#include <string>

namespace Citizens
{

	class Configuration
	{
	public:
		unsigned int resolution_x;
		unsigned int resolution_y;
		
		Configuration();
		
		//! I am a total douche, implementing a compiler gag
		//! because IScene::init() takes it as a parameter
		//! but not every inherited class needs it
		//! so the compiler complains
		void do_nothing(void) const {}
		
		template<class value_t>
		bool setProperty(const std::string& name,const value_t& value);
		
		template<class value_t>
		bool getProperty(const std::string& name,value_t& result) const;
		
		std::map<std::string,void*> properties;
	};

	//! \todo TYPE CHECKING!
	template<class value_t>
	bool Configuration::setProperty(const std::string& name,const value_t& value)
	{
		bool found = false;
		
		/*
		
		// hacky way to do it! huzzah
		(found = (properties.find(name) != properties.end())) &&
		*(value_t*)(properties[name]) = value;
		
		 */
		
		if(properties.find(name) != properties.end())
		{
			found = true;
			*(value_t*)(properties[name]) = value;
		}
		
		return found;
	}

	//! \todo TYPE CHECKING!
	template<class value_t>
	bool Configuration::getProperty(const std::string& name,value_t& result) const
	{
		bool found = false;
		
		
		if(properties.find(name) != properties.end())
		{
			found = true;
			result = *(value_t*)(properties.find(name)->second);
		}
		
		return found;
	}

};
#endif
