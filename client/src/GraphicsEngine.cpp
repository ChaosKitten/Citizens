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

GraphicsEngine::GraphicsEngine(Network* network)
{
	net = network;
}

irr::core::string<wchar_t> vec2str(irr::core::vector3df vec)
{
	irr::core::string<wchar_t> str(vec.X);
	str += irr::core::string<wchar_t>(vec.Y);
	str += irr::core::string<wchar_t>(vec.Z);
	return str;
}

bool GraphicsEngine::init(void)
{
	unsigned int xres,yres;
	if(	!config.getProperty("xres",xres) ||
		!config.getProperty("yres",yres))
	{
		running = false;
		return false;
	}
	//! \todo GraphicsEngine::init() should probably check that xres and yres are not > some threshold
	//!								 but I don't know what that threshold should be.
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

void GraphicsEngine::shutdown(void)
{
	running = false;
}

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

Configuration& GraphicsEngine::getConfig(void)
{
	return config;
}

Network* GraphicsEngine::getNetwork(void)
{
	return net;
}
