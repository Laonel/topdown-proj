#include "Core.h"

#include "Filesystem/Assets/AssetManager.h"
#include "Physics/PhysicsManager.h"
#include "Scene/Scene.h"

bool Core::s_initialised = false;

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

void Core::handleEvent()
{

}

void Core::update(const sf::Time& dt)
{

}

void Core::draw()
{

}
