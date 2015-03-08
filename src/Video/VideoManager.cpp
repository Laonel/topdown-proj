#include "Video/VideoManager.h"
#include "Filesystem/Configuration.h"

sf::RenderWindow* VideoManager::s_wnd = 0;
sf::VideoMode* VideoManager::s_mode = 0;
sf::ContextSettings VideoManager::s_context;
sf::String VideoManager::s_wndTitle = "TopDown RPG Project";

void VideoManager::init()
{

}

void VideoManager::shutdown()
{
	DELETE_OBJECT(s_wnd)
	DELETE_OBJECT(s_mode)
}

bool VideoManager::createWindow()
{
	if (s_wnd)
	{
		PRINT_ERROR << "Window is already created." << std::endl;
		return false;
	}

	// make sure we have a null pointer before creating new object
	DELETE_OBJECT(s_mode)

	s_context.depthBits = Configuration::Video::DepthBuffer;
	s_context.stencilBits = Configuration::Video::StencilBuffer;
	s_context.antialiasingLevel = Configuration::Video::Antialiasing;

	s_mode = new sf::VideoMode(Configuration::General::ScreenWidth,
						   	   Configuration::General::ScreenHeight,
						   	   Configuration::General::Colors);

	sf::Uint32 wndStyle;
	if (Configuration::General::Fullscreen)
		wndStyle = sf::Style::Fullscreen;
	else
		wndStyle = sf::Style::Titlebar;

	s_wnd = new sf::RenderWindow(*s_mode, s_wndTitle, wndStyle, s_context);

	if (!s_wnd->isOpen())
		return false;

	return true;
}

void VideoManager::destroyWindow()
{
	if (!s_wnd)
	{
		PRINT_ERROR << "No window was created, nothing to destroy." << std::endl;
		return;
	}

	s_wnd->close();

	DELETE_OBJECT(s_wnd);
	DELETE_OBJECT(s_mode);
}

sf::RenderWindow* VideoManager::getWindowHandle()
{
	return s_wnd;
}

bool VideoManager::isWindowOpen()
{
	if (!s_wnd)
		return false;

	return s_wnd->isOpen();
}

void VideoManager::display()
{
	s_wnd->display();
}

void VideoManager::clear(const sf::Color& c)
{
	s_wnd->clear(c);
}
