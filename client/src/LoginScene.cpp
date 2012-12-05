#include "LoginScene.hpp"
#include "ElementIDs.h"

using namespace Citizens;

LoginScene::LoginScene(irr::scene::ISceneManager& sm) : scenemgr(sm),ready(false) {}

/**
 * \brief initialises the scene
 * \details sets up a camera, gui and a background image for the Login screen
 * \param[in] config a Configuration reference we can use to ascertain
 * things like the x and y resolution of the screen we're drawing to
 */
void LoginScene::init(const Configuration& config)
{
	irr::scene::ICameraSceneNode* camera = scenemgr.addCameraSceneNode();
	scene_nodes["camera"] = camera;
	camera->setPosition(irr::core::vector3df(5,0,0));
	camera->setTarget(irr::core::vector3df(0,0,0));
	
	irr::gui::IGUIEnvironment* gui = scenemgr.getGUIEnvironment();
	
	irr::core::string<wchar_t> title(L"Citizens Login");
	irr::core::string<wchar_t> username(L"Username:");
	irr::core::string<wchar_t> password(L"Password:");
	
	irr::video::SColor label_color(0xFFFF0000);
	// somewhat counterintuitively, this is also the background for edit boxes
	gui->getSkin()->setColor(irr::gui::EGDC_WINDOW,0xCCFFFFFF);
	
	unsigned int xres;
	unsigned int yres;
	config.getProperty("xres",xres);
	config.getProperty("yres",yres);
	
	// first job: draw a background image
	irr::scene::IBillboardSceneNode* bg_billboard = scenemgr.addBillboardSceneNode();
	scene_nodes["background"] = bg_billboard;
	bg_billboard->setMaterialFlag(irr::video::EMF_LIGHTING,false);
	bg_billboard->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);
	bg_billboard->setMaterialTexture(0,scenemgr.getVideoDriver()->getTexture("art/stills/login.png"));
	
	irr::gui::IGUIFont* font = gui->getSkin()->getFont();
	
	irr::gui::IGUIStaticText* title_stext = gui->addStaticText(title.c_str(),
	irr::core::rect<irr::s32>(
	0, // start_x
	10, // start_y
	xres, // end_x
	font->getDimension(title.c_str()).Height + 10) // end_y
	,false,true);
	
	title_stext->setTextAlignment(irr::gui::EGUIA_CENTER,irr::gui::EGUIA_CENTER);
	title_stext->setOverrideColor(label_color);
	
	irr::s32 username_start_x,username_start_y;
	username_start_x = (xres/2) - (font->getDimension(username.c_str()).Width + 10)/2;
	username_start_y = (yres/2) - (font->getDimension(username.c_str()).Height + 50);
	
	irr::gui::IGUIStaticText* username_label = gui->addStaticText(
		username.c_str(),
		irr::core::rect<irr::s32>(
			username_start_x,
			username_start_y,
			username_start_x + (font->getDimension(title.c_str()).Width + 10),
			username_start_y + (font->getDimension(title.c_str()).Height + 10)
		),
		false,
		true
	);
	
	username_label->setOverrideColor(label_color);
	
	irr::core::rect<irr::s32> username_box_dim(
			username_start_x - 5,
			username_start_y + (font->getDimension(username.c_str()).Height + 10),
			username_start_x + (font->getDimension(username.c_str()).Width + 5),
			username_start_y + (font->getDimension(username.c_str()).Height + 10) + 25
	);
	
	irr::gui::IGUIEditBox* username_box = gui->addEditBox(
		L"",
		username_box_dim,
		true,
		0,
		USERNAME_BOX_ID
	);
	
	username_box->setOverrideFont(font);
	
	irr::s32 password_start_x,password_start_y;
	password_start_x = username_box_dim.UpperLeftCorner.X + 5;
	password_start_y = username_box_dim.LowerRightCorner.Y + 10;
	
	irr::gui::IGUIStaticText* password_label = gui->addStaticText(
		password.c_str(),
		irr::core::rect<irr::s32>(
			password_start_x,
			password_start_y,
			password_start_x + (font->getDimension(password.c_str()).Width + 10),
			password_start_y + (font->getDimension(password.c_str()).Height + 10)
		),
		false,
		true
	);
	password_label->setOverrideColor(label_color);
	
	irr::gui::IGUIEditBox* password_box = gui->addEditBox(
		L"",
		irr::core::rect<irr::s32>(
			password_start_x - 5,
			password_start_y + (font->getDimension(password.c_str()).Height + 10),
			password_start_x + (font->getDimension(password.c_str()).Width + 5),
			password_start_y + (font->getDimension(password.c_str()).Height + 10) + 25
		),
		true,
		0,
		PASSWORD_BOX_ID
	);
	
	password_box->setPasswordBox(true);
	
	// AND FINALLY, the login button!
	int login_button_height = 25;
	int login_button_width = 70;
	// uncomment the following line if you find a use for storing the login button in a local variable
	//irr::gui::IGUIButton* loginButton = 
	gui->addButton(
		irr::core::rect<irr::s32>(
			password_start_x,
			password_start_y + (font->getDimension(password.c_str()).Height + 10) + 30,
			password_start_x + login_button_width,
			password_start_y + (font->getDimension(password.c_str()).Height + 10) + 30 + login_button_height + 5
		),
		0,
		LOGIN_BUTTON_ID,
		L"Login",
		L"Cross your fingers!"
	);
	
	// quit button stuff
	int button_width = 70;
	int button_height = 25;
	// uncomment the following line if you find a use for storing the quit button in a local variable
	//irr::gui::IGUIButton* quitButton = 
	gui->addButton(
		irr::core::rect<irr::s32>(
			xres-(button_width+10),
			yres-(button_height+15),
			xres-10,
			yres-10
		),
		0,
		QUIT_BUTTON_ID,
		L"Quit",
		L"Leaving, so soon?"
	);
	ready = true;
}

void LoginScene::unload(void)
{
	for(scene_node_it i = scene_nodes.begin(); i != scene_nodes.end(); ++i)
	{
		i->second->remove();
	}
	scenemgr.getGUIEnvironment()->clear();
}

/**
 * \brief whether or not the scene has been "loaded" or not
 * \details used to simplify the GraphicsEngine flow
 * \return true if the scene has been loaded, false otherwise
 */
bool LoginScene::is_setup(void)
{
	return ready;
}

/**
 * \brief I forget what this function is for, I think it was originally to
 * shut the compiler up about 'scenemgr' being unused.  I think it's
 * redundant now.  The only clue to its usage is a code comment
 * that reads "I feel like a collosal dick for doing this, but oh well".
 * Man, I should really get better at commenting things.
 * \warning probably getting deleted sometime soon
 */
// I feel like a collosal dick for doing this, but oh well
irr::scene::ISceneManager& LoginScene::operator()()
{
	return scenemgr;
}
