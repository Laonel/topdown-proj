#include "Filesystem/Configuration.h"

int Configuration::General::Colors;
bool Configuration::General::Fullscreen;
int Configuration::General::ScreenWidth;
int Configuration::General::ScreenHeight;
bool Configuration::General::TestMode;

int Configuration::Video::Antialiasing;
int Configuration::Video::DepthBuffer;
int Configuration::Video::StencilBuffer;

void Configuration::parseConfig(ConfigFile* cfg)
{
	
}
