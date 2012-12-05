#include "TestNetwork.hpp"

#pragma GCC diagnostic ignored "-Wunused-parameter"

using namespace Citizens;

bool TestNetwork::connect(const std::string& ip)
{
	return true;
}

bool TestNetwork::send(NetworkCommand c,char payload_length,const std::string& payload)
{
	return true;
}

bool TestNetwork::login(const std::string& username, const std::string& password)
{
	return true;
}

std::string TestNetwork::get_error(void)
{
	return "no error";
}

bool TestNetwork::is_disconnected(void)
{
	return false;
}

void TestNetwork::get_pos(Coordinate& c)
{
	c.zone_x = 0;
	c.zone_y = 0;
	c.area_x = 0;
	c.area_y = 0;
	c.pos_x = 0;
	c.pos_y = 0;
	c.pos_z = 10;
	c.dir_x = 180.0f;
	c.dir_y = 0;
	c.dir_z = 0;
}
#pragma GCC diagnostic pop
