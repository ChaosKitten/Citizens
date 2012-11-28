#include "EventReceiver.hpp"
#include "ElementIDs.h"
#include <iostream>

using namespace Citizens;

EventReceiver::EventReceiver(GraphicsEngine& ge,irr::scene::ISceneManager& sm) : engine(ge),scenemgr(sm) {}

// ignore -Wswitch for this function, as there are lots of event types we just ignore
#pragma GCC diagnostic ignored "-Wswitch"
bool EventReceiver::OnEvent(const irr::SEvent& event)
{
	bool processed = false;
	
	switch(event.EventType)
	{
		case irr::EET_GUI_EVENT:
		{
			switch(event.GUIEvent.EventType)
			{
				case irr::gui::EGET_BUTTON_CLICKED:
				{
					switch(event.GUIEvent.Caller->getID())
					{
						case LOGIN_BUTTON_ID:
						{
							irr::gui::IGUIEnvironment* gui = scenemgr.getGUIEnvironment();
							irr::gui::IGUIElement* user_box = gui->getRootGUIElement()->getElementFromId(USERNAME_BOX_ID);
							irr::gui::IGUIElement* pass_box = gui->getRootGUIElement()->getElementFromId(PASSWORD_BOX_ID);
							if(!user_box || !pass_box)
							{
								std::cerr << "Warning: Could not find username and password input boxes for 'login' procedure." << std::endl;
								break;
							}
							const wchar_t* username = user_box->getText();
							const wchar_t* password = pass_box->getText();
							irr::core::stringc usertmp = username;
							const char* username_cstr = usertmp.c_str();
							irr::core::stringc passtmp = password;
							const char* password_cstr = passtmp.c_str();
							engine.getNetwork()->login(username_cstr,password_cstr);
							break;
						}
						case QUIT_BUTTON_ID:
						{
							engine.shutdown();
							processed = true;
							break;
						}
					}
				}
			}
			
			break;
		}
	}
	
	return processed;
}
#pragma GCC diagnostic pop
