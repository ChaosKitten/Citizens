#ifndef GRAPHICS_ENGINE_HPP
#define GRAPHICS_ENGINE_HPP

#include "Network.hpp"
#include "Configuration.hpp"
#include "IScene.hpp"
#include "Coordinate.hpp"
#include <map>
#include <string>
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <irrlicht.h>
#pragma GCC diagnostic pop

namespace Citizens
{
	struct Player
	{
		Coordinate position;
	};
	class GraphicsEngine
	{
	public:
		GraphicsEngine(Network* network);
		bool init(void);
		bool running;
		void render(void);
		bool set(const std::string& property, const std::string& value);
		void shutdown(void);
		Network* getNetwork(void);
		Configuration& getConfig(void);
		void change_scene(IScene* new_scene);
		irr::IrrlichtDevice* get_device(void) { return screen; };
		
		Player player;
		
	private:
		Configuration config;
		static inline bool is_valid(const std::string& property);
		Network* net;
		IScene* current_scene;
		
		// irrlicht stuff
		irr::IrrlichtDevice* screen;
		irr::video::IVideoDriver* driver;
		irr::scene::ISceneManager* scenemgr;
		irr::gui::IGUIEnvironment *gui;
		irr::scene::ICameraSceneNode* camera;
		irr::gui::IGUIStaticText* camera_pos;
		
	};
};

#endif
