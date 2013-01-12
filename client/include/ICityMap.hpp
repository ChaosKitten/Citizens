#ifndef ICITY_MAP_HPP
#define ICITY_MAP_HPP

#include "ISerializable.hpp"

namespace citi
{
	class ICityMap : public ISerializable<ICityMap>
	{
	public:
		virtual void render(void) = 0;
		virtual citi::TSerial serialize(void) = 0;
		virtual void deserialize(citi::TSerial s) = 0;
	};
};

#endif
