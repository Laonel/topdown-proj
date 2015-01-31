#ifndef _CORE_H_
#define _CORE_H_

#include "Utils.h"
#include "Math/Math.h"

#include "Filesystem/Assets/AssetManager.h"
#include "Filesystem/ConfigFile.h"
#include "Filesystem/Configuration.h"
#include "Filesystem/Serialization.h"

#include "Scene/Scene.h"

#include "Physics/PhysicsManager.h"

#include "Video/VideoManager.h"

class Core
{
public:

	static bool init();

	static void shutdown();

	static bool isInit();

	static void handleEvent();

	static void update(const sf::Time& dt);

	static void draw();

private:

	static bool s_initialised;

};

#endif
