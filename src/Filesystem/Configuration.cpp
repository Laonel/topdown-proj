#include "Filesystem/Configuration.h"
#include "Filesystem/ConfigFile.h"

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
	General::Colors = cfg->getInt("General.Colors", 32);
	General::Fullscreen = cfg->getBoolean("General.WindowMode", 0);
	General::ScreenWidth = cfg->getInt("General.Width", 800);
	General::ScreenHeight = cfg->getInt("General.Height", 600);
	General::TestMode = cfg->getBoolean("General.TestMode", 0);

	Video::Antialiasing = cfg->getInt("Video.Antialiasing", 8);
	Video::DepthBuffer = cfg->getInt("Video.DepthBuffer", 32);
	Video::StencilBuffer = cfg->getInt("Video.StencilBuffer", 32);
}

void Configuration::applyConfig()
{
	
}
