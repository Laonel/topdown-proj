#include "Core.h"

#include "Filesystem/Assets/AssetManager.h"
#include "Scene/Scene.h"

bool Core::s_initialised = false;

bool Core::init()
{
	// initialise AssetManager
	// it just creates default texture as null image
	AssetManager::init();
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

void Core::shutdown()
{

}

void Core::applyConfig()
{

}
