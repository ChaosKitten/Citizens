#ifndef CLOGIN_MENU_PAGE_HPP
#define CLOGIN_MENU_PAGE_HPP

#include "IMenuPage.hpp"
namespace citi
{
	class CLoginMenuPage : public IMenuPage
	{
	public:
		CLoginMenuPage();
		~CLoginMenuPage();
		void render(void);
	};
};

#endif
