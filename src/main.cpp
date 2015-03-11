#include "Utils.h"
#include "Core.h"

#include <cstdio>

#include <Scene/Map/MapLoader.h>

int main()
{
    // read config file before calling init() function
    ConfigFile cfgFile;
    cfgFile.readConfig("../assets/config.cfg");
    Configuration::parseConfig(&cfgFile);

    Core::init();

    VideoManager::createWindow();

    MapLoader ml("maps/");
    ml.load("desert.tmx");

    while (!Core::shouldQuit())
    {
        Core::handleEvent();

        ml.updateQuadTree(sf::FloatRect(0.f, 0.f, 800.f, 600.f));
        sf::Vector2f mousePos = VideoManager::getWindowHandle()->mapPixelToCoords(sf::Mouse::getPosition(*VideoManager::getWindowHandle()));
        std::vector<MapObject*> objects = ml.queryQuadTree(sf::FloatRect(mousePos.x - 10.f, mousePos.y - 10.f, 20.f, 20.f));

        std::stringstream stream;
        stream << "Query object count: " << objects.size();
        VideoManager::getWindowHandle()->setTitle(stream.str());

        VideoManager::clear();
        
        VideoManager::getWindowHandle()->draw(ml);
        ml.draw(*VideoManager::getWindowHandle(), MapLayer::Debug);

        VideoManager::display();
    }

    Core::shutdown();

    return 0;
}
