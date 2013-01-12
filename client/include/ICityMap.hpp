#ifndef ICITY_MAP_HPP
#define ICITY_MAP_HPP

#include "ISerializable.hpp"

namespace citi
{
	class ICityMap : public ISerializable<ICityMap>
	{
	public:
		virtual void render(void) = 0;
		virtual void serialize(void) = 0;
		virtual ICityMap* deserialize(void) = 0;
	};
};

#endif
