#include "GraphicsEngine.hpp"
#include "Configuration.hpp"
#include "LoginScene.hpp"
#include "EventReceiver.hpp"

#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <irrlicht.h>
#pragma GCC diagnostic pop
#include <iostream>
#include <string>
#include <sstream>

using namespace Citizens;

#define NUM_PROPERTIES 1

/**
 * \brief ctor
 * \param[in] network a network connection object (assumed to be blank)
 * \todo convert network parameter and field to reference from pointer
 */
GraphicsEngine::GraphicsEngine(Network* network)
{
	net = network;
}

/**
 * \brief local helper function for getting string representations of vectors
 * \details returns XYZ from the vector (no comma-delimiting is performed)
 * This is mostly for debugging, not really recommended for use in production code
 * \param[in] vec the vector to convert
 * \return an irr::core::string<wchar_t> representation of the vector
 */
irr::core::string<wchar_t> vec2str(irr::core::vector3df vec)
{
	irr::core::string<wchar_t> str(vec.X);
	str += irr::core::string<wchar_t>(vec.Y);
	str += irr::core::string<wchar_t>(vec.Z);
	return str;
}

/**
 * \brief initialises the graphics engine
 * \details creates an irrlicht 'device' to render on and stores references to
 * the GUIEnvironment, video driver and SceneManager fields from the device
 * Then adds the EventReceiver object as an event receiver
 * Then loads the default font and applies it to the skin
 * Finally runs the "login" scene ready for rendering on the first loop
 * \return a boolean indicating whether the initialisation succeeded or not
 */
bool GraphicsEngine::init(void)
{
	unsigned int xres,yres;
	if(	!config.getProperty("xres",xres) ||
		!config.getProperty("yres",yres))
	{
		running = false;
		return false;
	}
	//! \todo should probably check that xres and yres are not > some threshold
	//!		  but I don't know what that threshold should be.
	if(xres == 0 || yres == 0)
	{
		running = false;
		return false;
	}
	running = true;
	screen = irr::createDevice(irr::video::EDT_OPENGL,irr::core::dimension2d<irr::u32>(xres,yres));
	driver = screen->getVideoDriver();
	scenemgr = screen->getSceneManager();
	gui = screen->getGUIEnvironment();
	LoginScene login(*scenemgr);
	
	screen->setEventReceiver(new EventReceiver(*this,*scenemgr));
	
	irr::gui::IGUIFont* default_font = gui->getFont("fonts/myfont.xml");
	
	irr::gui::IGUISkin* skin = gui->getSkin();
	skin->setFont(default_font);
	
	// run our login scene
	login.init(config);

	
	/*
	camera_pos = gui->addStaticText(vec2str(camera->getAbsolutePosition()).c_str(),irr::core::rect<irr::s32>(40,10,400,50),true,true);
	camera_pos->setOverrideColor(irr::video::SColor(255,255,255,255));
    */
    
	return true;
}

/**
 * \brief shuts down the engine (sets 'running' to false)
 */
void GraphicsEngine::shutdown(void)
{
	running = false;
}

/**
 * \brief renders one frame of video to the screen
 * \details renders GUI and background, this may in future be split
 * into two functions, one for GUI elements and one for the 3D "world view"
 * So that the user can "disable" the GUI at any point, within any scene.
 * On the other hand, I may keep it like this and just "hide" the GUI
 * from within the scene, as some scenes may want to choose not to allow
 * the user to hide the GUI (such as the login screen, which is rather meaningless
 * without a GUI).
 */
void GraphicsEngine::render(void)
{
	if(!running)
	{
		screen->drop();
		return;
	}
	if(!screen->isWindowActive()) screen->yield();
	else
	{
		driver->beginScene(true, true, irr::video::SColor(0xFF000000));
		scenemgr->drawAll();
		gui->drawAll();
		driver->endScene();
	}
	if((!screen->run() || !driver))
	{
		this->shutdown();
	}
}

/**
 * \brief returns a reference to the GE's Configuration
 */
Configuration& GraphicsEngine::getConfig(void)
{
	return config;
}

/**
 * \brief returns a pointer to the GE's Network
 * \note in future versions this may return a reference rather than a pointer
 */
Network* GraphicsEngine::getNetwork(void)
{
	return net;
}
