#ifndef EVENTRECEIVER_HPP
#define EVENTRECEIVER_HPP

#include "GraphicsEngine.hpp"

#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <irrlicht.h>
#pragma GCC diagnostic pop

namespace Citizens
{
	class EventReceiver : public irr::IEventReceiver
	{
	public:
		EventReceiver(GraphicsEngine& ge,irr::scene::ISceneManager& sm);
		bool OnEvent(const irr::SEvent& event);
	private:
		GraphicsEngine& engine;
		irr::scene::ISceneManager& scenemgr;
	};
	
};



#endif
