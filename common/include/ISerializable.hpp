#ifndef ISERIALIZABLE_HPP
#define ISERIALIZABLE_HPP

namespace citi
{
	template<class InheritedType>
	class ISerializable
	{
	public:
		virtual void serialize(void) = 0;
		virtual InheritedType* deserialize(void) = 0;
	};
};

#endif
