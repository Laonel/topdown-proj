#ifndef _VIDEO_MANAGER_H_
#define _VIDEO_MANAGER_H_

#include "Utils.h"

#include <SFML/Graphics.hpp>

class VideoManager
{
public:

	static void init();

	static void shutdown();

	static bool createWindow();

	static void destroyWindow();

	static sf::RenderWindow* getWindowHandle();

	static bool isWindowOpen();

	static void display();

	static void clear(const sf::Color& c = sf::Color::Black);

private:

	static sf::RenderWindow* s_wnd;
	static sf::VideoMode* s_mode;

	static sf::ContextSettings s_context;

	static sf::String s_wndTitle;

};

#endif
