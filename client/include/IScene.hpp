#ifndef SCENE_HPP
#define SCENE_HPP

#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <irrlicht.h>
#pragma GCC diagnostic pop

#include "Configuration.hpp"

namespace Citizens
{
	class IScene
	{
	public:
		virtual void init(const Configuration& config) = 0;
		virtual bool is_setup(void) = 0;
		virtual void unload(void) = 0;
	};
};

#endif
