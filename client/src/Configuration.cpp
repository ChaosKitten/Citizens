#include "Configuration.hpp"

using namespace Citizens;

Configuration::Configuration() :
properties ({
		{"xres",&resolution_x},
		{"yres",&resolution_y}
	})
	{}
