#include "Utils.h"
#include "Core.h"

#include <cstdio>

int main()
{
    // read config file before calling init() function
    ConfigFile cfgFile;
    cfgFile.readConfig("../assets/config.cfg");
    Configuration::parseConfig(&cfgFile);

    Core::init();

    VideoManager::createWindow();

    Core::shutdown();

    return 0;
}
