#ifndef _ASSET_MANAGER_H_
#define _ASSET_MANAGER_H_

#include "Utils.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Music.hpp>

class AssetManager
{
public:

	static void init();

	static sf::Image* addImage(const std::string& id, const sf::Image* ptr);
	static sf::Texture* addTexture(const std::string& id, const sf::Texture* ptr);
	static sf::Font* addFont(const std::string& id, const sf::Font* ptr);
	static sf::Shader* addShader(const std::string& id, const sf::Shader* ptr);
	static sf::Sound* addSound(const std::string& id, const sf::SoundBuffer* ptrBuff, const sf::Sound* ptr);
	static sf::Music* addMusic(const std::string& id, const sf::Music* ptr);

	static sf::Image* loadImage(const std::string& id, const std::string& path);
	static sf::Texture* loadTexture(const std::string& id, const std::string& path);
	static sf::Font* loadFont(const std::string& id, const std::string& path);
	static sf::Shader* loadShader(const std::string& id, const std::string& vspath, const std::string& fspath, const std::string* uniforms = 0, uint32 uniformsCount = 0);
	static sf::Sound* loadSound(const std::string& id, const std::string& path);
	static sf::Music* loadMusic(const std::string& id, const std::string& path);

	static sf::Image* getImage(const std::string& id);
	static sf::Texture* getTexture(const std::string& id);
	static sf::Font* getFont(const std::string& id);
	static sf::Shader* getShader(const std::string& id);
	static sf::Sound* getSound(const std::string& id);
	static sf::Music* getMusic(const std::string& id);

	static std::vector<std::string>* getShaderUniforms(const std::string& id);

	static inline sf::Texture* getDefaultTexture() { return s_defaultTexture; }

	static std::string findImage(const sf::Image* ptr);
	static std::string findTexture(const sf::Texture* ptr);
	static std::string findFont(const sf::Font* ptr);
	static std::string findShader(const sf::Shader* ptr);
	static std::string findSound(const sf::Sound* ptr);
	static std::string findMusic(const sf::Music* ptr);

	static void releaseImage(const std::string& id);
	static void releaseTexture(const std::string& id);
	static void releaseFont(const std::string& id);
	static void releaseShader(const std::string& id);
	static void releaseSound(const std::string& id);
	static void releaseMusic(const std::string& id);

	static void releaseAllImages();
	static void releaseAllTextures();
	static void releaseAllFonts();
	static void releaseAllShaders();
	static void releaseAllSounds();
	static void releaseAllMusics();

	static void releaseAll();

private:

	static std::map<std::string, sf::Image*> s_images;
	static std::map<std::string, sf::Texture*> s_textures;
	static std::map<std::string, sf::Font*> s_fonts;
	static std::map<std::string, sf::Shader*> s_shaders;
	static std::map<std::string, sf::SoundBuffer*> s_soundBuffs;
	static std::map<std::string, sf::Sound*> s_sounds;
	static std::map<std::string, sf::Music*> s_musics;

	static std::map<std::string, std::string> s_metaImages;
	static std::map<std::string, std::string> s_metaTextures;
	static std::map<std::string, std::string> s_metaFonts;
	static std::map<std::string, std::string> s_metaShaders;
	static std::map<std::string, std::string> s_metaSounds;
	static std::map<std::string, std::string> s_metaMusics;

	static std::map<std::string, std::vector<std::string>> s_shadersUniforms;

	static sf::Texture* s_defaultTexture;
};

#endif
