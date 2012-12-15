#include "EventReceiver.hpp"
#include "ElementIDs.h"
#include "MapScene.hpp"
#include <iostream>

using namespace Citizens;

EventReceiver::EventReceiver(GraphicsEngine& ge,irr::scene::ISceneManager& sm) : engine(ge),scenemgr(sm) {}

/**
 * \brief handles the event passed in
 * \details this event handler function does not defer to any helper functions
 * this design ought to change as the number of events handled grows
 * you can see 3 nested 'switch' statements as a result of this bad design
 * already, and it's only gonig to get worse
 * \param[in] event the event that triggered the execution of this function
 * \return a boolean representing whether or not the function was able to
 * handle the event (so that instrumentation in code can report unhandled events)
 * \note another ugly thing is that I used GCC extensions to turn off -Wswitch for this
 * function only - fortunately, most non-GCC compilers should either ignore the #pragma
 * directive or at worst issue a diagnostic about an unrecognised #pragma directive
 */
// ignore -Wswitch for this function, as there are lots of event types we just ignore
#pragma GCC diagnostic ignored "-Wswitch"
bool EventReceiver::OnEvent(const irr::SEvent& event)
{
	bool processed = false;
	
	switch(event.EventType)
	{
		case irr::EET_KEY_INPUT_EVENT:
		{
			switch(event.KeyInput.Key)
			{
				case irr::KEY_ESCAPE:
				{
					//! \todo handle irr::KEY_ESCAPE a little more delicately than just killing the whole engine
					engine.shutdown();
					processed = true;
					break;
				}
			}
		}
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
							bool logged_in = engine.getNetwork()->login(username_cstr,password_cstr);
							engine.getNetwork()->get_pos(engine.player.position);
							if(logged_in)
								engine.change_scene(new MapScene(engine,engine.player.position,scenemgr,*engine.get_device()));
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
