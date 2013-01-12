#ifndef ISERIALIZABLE_HPP
#define ISERIALIZABLE_HPP

namespace citi
{
	typedef unsigned long int TSerial;
	template<class InheritedType>
	class ISerializable
	{
	public:
		virtual TSerial serialize(void) = 0;
		virtual void deserialize(TSerial s) = 0;
	};
};

#endif
