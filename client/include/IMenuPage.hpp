#ifndef IMENU_PAGE_HPP
#define IMENU_PAGE_HPP

#include <string>
namespace citi
{
	class IMenuPage
	{
	public:
		virtual void render(void) = 0;
	protected:
	};
};

#endif
