#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include "Filesystem/ConfigFile.h"

class Configuration
{
public:

	static void parseConfig(ConfigFile* cfg);

	static struct General
	{
		static int Colors;
		static bool Fullscreen;
		static int ScreenWidth;
		static int ScreenHeight;
		static bool TestMode;
	};

	static struct Video
	{
		static int Antialiasing;
		static int DepthBuffer;
		static int StencilBuffer;
	};

};

#endif
