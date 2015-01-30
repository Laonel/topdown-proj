#ifndef _CORE_H_
#define _CORE_H_

#include "Utils.h"
#include "Math/Math.h"

class Core
{
public:

	static bool init();

	static bool isInit();

	static void handleEvent();

	static void update(const sf::Time& dt);

	static void draw();

	static void shutdown();

	static bool createWindow() {}

	static void destroyWindow() {}

	static void applyConfig();

private:

	static bool s_initialised;

};

#endif
