#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

class ConfigFile;

class Configuration
{
public:

	static void parseConfig(ConfigFile* cfg);

	static void applyConfig();

	struct General
	{
		static int Colors;
		static bool Fullscreen;
		static int ScreenWidth;
		static int ScreenHeight;
		static bool TestMode;
	};

	struct Video
	{
		static int Antialiasing;
		static int DepthBuffer;
		static int StencilBuffer;
	};

};

#endif
