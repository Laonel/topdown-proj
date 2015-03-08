#include "Core.h"

#include "Filesystem/Assets/AssetManager.h"
#include "Physics/PhysicsManager.h"
#include "Scene/Scene.h"

#include <SFML/Window.hpp>

bool Core::s_initialised = false;
bool Core::s_shouldClose = false;

bool Core::init()
{
	// initialise AssetManager
	// it just creates default texture as null image
	AssetManager::init();

	PhysicsManager::init();

	VideoManager::init();

	Scene::init();

	s_initialised = true;
}

void Core::shutdown()
{
	if (s_initialised && !s_shouldClose)
	{
		s_shouldClose = true;
		return;
	}

	Scene::shutdown();

	VideoManager::shutdown();

	PhysicsManager::shutdown();

	AssetManager::shutdown();

	s_initialised = false;
}

bool Core::isInit()
{
	return s_initialised;
}

bool Core::shouldQuit()
{
	return s_shouldClose;
}

void Core::handleEvent()
{
	sf::Event e;
	while (VideoManager::getWindowHandle()->pollEvent(e))
	{
		if (e.type == sf::Event::Closed)
			shutdown();
	}
}

void Core::update(const sf::Time& dt)
{

}

void Core::draw()
{

}
