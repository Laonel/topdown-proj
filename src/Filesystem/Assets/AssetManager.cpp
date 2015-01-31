#include "Filesystem/Assets/AssetManager.h"

std::map<std::string, sf::Image*> AssetManager::s_images;
std::map<std::string, sf::Texture*> AssetManager::s_textures;
std::map<std::string, sf::Font*> AssetManager::s_fonts;
std::map<std::string, sf::Shader*> AssetManager::s_shaders;
std::map<std::string, sf::SoundBuffer*> AssetManager::s_soundBuffs;
std::map<std::string, sf::Sound*> AssetManager::s_sounds;
std::map<std::string, sf::Music*> AssetManager::s_musics;

std::map<std::string, std::string> AssetManager::s_metaImages;
std::map<std::string, std::string> AssetManager::s_metaTextures;
std::map<std::string, std::string> AssetManager::s_metaFonts;
std::map<std::string, std::string> AssetManager::s_metaShaders;
std::map<std::string, std::string> AssetManager::s_metaSounds;
std::map<std::string, std::string> AssetManager::s_metaMusics;

std::map<std::string, std::vector<std::string>> AssetManager::s_shadersUniforms;

sf::Texture* AssetManager::s_defaultTexture = new sf::Texture();

void AssetManager::init()
{
	s_defaultTexture->create(1, 1);
}

void AssetManager::shutdown()
{
	DELETE_OBJECT(s_defaultTexture)
}

sf::Image* AssetManager::addImage(const std::string& id, const sf::Image* ptr)
{
	sf::Image* i = getImage(id);
	if (!i && ptr)
	{
		i = (sf::Image*)(ptr);
		s_images[id] = i;
	}

	return i;
}

sf::Texture* AssetManager::addTexture(const std::string& id, const sf::Texture* ptr)
{
	sf::Texture* t = getTexture(id);
	if (!t && ptr)
	{
		t = (sf::Texture*)(ptr);
		s_textures[id] = t;
	}

	return t;
}

sf::Font* AssetManager::addFont(const std::string& id, const sf::Font* ptr)
{
	sf::Font* f = getFont(id);
	if (!f && ptr)
	{
		f = (sf::Font*)(ptr);
		s_fonts[id] = f;
	}

	return f;
}

sf::Shader* AssetManager::addShader(const std::string& id, const sf::Shader* ptr)
{
	sf::Shader* s = getShader(id);
	if (!s && ptr)
	{
		s = (sf::Shader*)(ptr);
		s_shaders[id] = s;
	}

	return s;
}

sf::Sound* AssetManager::addSound(const std::string& id, const sf::SoundBuffer* ptrBuff, const sf::Sound* ptr)
{
	sf::Sound* s = getSound(id);
	if (!s && ptrBuff && ptr)
	{
		s = (sf::Sound*)(ptr);
		s_soundBuffs[id] = (sf::SoundBuffer*)(ptrBuff);
		s_sounds[id] = s;
	}

	return s;
}

sf::Music* AssetManager::addMusic(const std::string& id, const sf::Music* ptr)
{
	sf::Music* m = getMusic(id);
	if (!m && ptr)
	{
		m = (sf::Music*)(ptr);
		s_musics[id] = m;
	}

	return m;
}

sf::Image* AssetManager::loadImage(const std::string& id, const std::string& path)
{
	sf::Image* i = getImage(id);
	if (!i)
	{
		i = new sf::Image();

		if (!i->loadFromFile(path))
		{
			DELETE_OBJECT(i);
			return 0;
		}

		s_images[id] = i;
		s_metaImages[id] = path;
	}

	return i;
}

sf::Texture* AssetManager::loadTexture(const std::string& id, const std::string& path)
{
	sf::Texture* t = getTexture(id);
	if (!t)
	{
		t = new sf::Texture();

		if (!t->loadFromFile(path))
		{
			DELETE_OBJECT(t);
			return 0;
		}

		s_textures[id] = t;
		s_metaTextures[id] =  path;
	}

	return t;
}

sf::Font* AssetManager::loadFont(const std::string& id, const std::string& path)
{
	sf::Font* f = getFont(id);
	if (!f)
	{
		f = new sf::Font();

		if (!f->loadFromFile(path))
		{
			DELETE_OBJECT(f);
			return 0;
		}

		s_fonts[id] = f;
		s_metaFonts[id] = path;
	}

	return f;
}

sf::Shader* AssetManager::loadShader(const std::string& id, const std::string& vspath, const std::string& fspath, const std::string* uniforms, uint32 uniformsCount)
{
	sf::Shader* s = getShader(id);
	if (!s)
	{
		s = new sf::Shader();
		if (!s->loadFromFile(vspath, fspath))
		{
			DELETE_OBJECT(s);
			return 0;
		}

		s_shaders[id] = s;
		s_metaShaders[id] = vspath + "|" + fspath;

		s_shadersUniforms[id].clear();
		for (uint32 i = 0; i < uniformsCount; ++i)
			s_shadersUniforms[id].push_back(uniforms[i]);
	}

	return s;
}

sf::Sound* AssetManager::loadSound(const std::string& id, const std::string& path)
{
	sf::Sound* s = getSound(id);
	if (!s)
	{
		s = new sf::Sound();
		sf::SoundBuffer* sb = new sf::SoundBuffer();
		if (!sb->loadFromFile(path))
		{
			DELETE_OBJECT(s);
			DELETE_OBJECT(sb);
			return 0;
		}

		s->setBuffer(*sb);
		s_soundBuffs[id] = sb;
		s_sounds[id] = s;
		s_metaSounds[id] = path;
	}

	return s;
}

sf::Music* AssetManager::loadMusic(const std::string& id, const std::string& path)
{
	sf::Music* m = getMusic(id);
	if (!m)
	{
		m = new sf::Music();
		if (!m->openFromFile(path))
		{
			DELETE_OBJECT(m);
			return 0;
		}

		s_musics[id] = m;
		s_metaMusics[id] = path;
	}

	return m;
}

sf::Image* AssetManager::getImage(const std::string& id)
{
	return s_images.count(id) ? s_images[id] : 0;
}

sf::Texture* AssetManager::getTexture(const std::string& id)
{
	return s_images.count(id) ? s_textures[id] : 0;
}

sf::Font* AssetManager::getFont(const std::string& id)
{
	return s_fonts.count(id) ? s_fonts[id] : 0;
}

sf::Shader* AssetManager::getShader(const std::string& id)
{
	return s_shaders.count(id) ? s_shaders[id] : 0;
}

sf::Sound* AssetManager::getSound(const std::string& id)
{
	return s_sounds.count(id) ? s_sounds[id] : 0;
}

sf::Music* AssetManager::getMusic(const std::string& id)
{
	return s_musics.count(id) ? s_musics[id] : 0;
}

std::vector<std::string>* AssetManager::getShaderUniforms(const std::string& id)
{
	return s_shadersUniforms.count(id) ? &s_shadersUniforms[id] : 0;
}

std::string AssetManager::findImage(const sf::Image* ptr)
{
	if (!ptr)
		return "";

	for (std::map<std::string, sf::Image*>::const_iterator it = s_images.begin(); it != s_images.end(); it++)
		if (it->second == ptr)
			return it->first;

	return "";
}

std::string AssetManager::findTexture(const sf::Texture* ptr)
{
	if (!ptr)
		return "";

	for (std::map<std::string, sf::Texture*>::const_iterator it = s_textures.begin(); it != s_textures.end(); it++)
		if (it->second == ptr)
			return it->first;

	return "";
}

std::string AssetManager::findFont(const sf::Font* ptr)
{
	if (!ptr)
		return "";

	for (std::map<std::string, sf::Font*>::const_iterator it = s_fonts.begin(); it != s_fonts.end(); it++)
		if (it->second == ptr)
			return it->first;

	return "";
}

std::string AssetManager::findShader(const sf::Shader* ptr)
{
	if (!ptr)
		return "";

	for (std::map<std::string, sf::Shader*>::const_iterator it = s_shaders.begin(); it != s_shaders.end(); it++)
		if (it->second == ptr)
			return it->first;

	return "";
}

std::string AssetManager::findSound(const sf::Sound* ptr)
{
	if (!ptr)
		return "";

	for (std::map<std::string, sf::Sound*>::const_iterator it = s_sounds.begin(); it != s_sounds.end(); it++)
		if (it->second == ptr)
			return it->first;

	return "";
}

std::string AssetManager::findMusic(const sf::Music* ptr)
{
	if (!ptr)
		return "";

	for (std::map<std::string, sf::Music*>::const_iterator it = s_musics.begin(); it != s_musics.end(); it++)
		if (it->second == ptr)
			return it->first;

	return "";
}

void AssetManager::releaseImage(const std::string& id)
{
	if (s_images.count(id))
		delete s_images[id];
}

void AssetManager::releaseTexture(const std::string& id)
{
	if (s_textures.count(id))
		delete s_textures[id];
}

void AssetManager::releaseFont(const std::string& id)
{
	if (s_fonts.count(id))
		delete s_fonts[id];
}

void AssetManager::releaseShader(const std::string& id)
{
	if (s_shaders.count(id))
		delete s_shaders[id];
}

void AssetManager::releaseSound(const std::string& id)
{
	if (s_sounds.count(id))
	{
		delete s_soundBuffs[id];
		delete s_sounds[id];
	}
}

void AssetManager::releaseMusic(const std::string& id)
{
	if (s_musics.count(id))
		delete s_musics[id];
}

void AssetManager::releaseAllImages()
{
	sf::Image* i;
	for (std::map<std::string, sf::Image*>::iterator it = s_images.begin(); it != s_images.end(); ++it)
	{
		if (it->second)
		{
			i = it->second;
			delete i;
		}
	}

	s_images.clear();
}

void AssetManager::releaseAllTextures()
{
	sf::Texture* t;
	for (std::map<std::string, sf::Texture*>::iterator it = s_textures.begin(); it != s_textures.end(); ++it)
	{
		if (it->second)
		{
			t = it->second;
			delete t;
		}
	}

	s_textures.clear();
}

void AssetManager::releaseAllFonts()
{
	sf::Font* f;
	for (std::map<std::string, sf::Font*>::iterator it = s_fonts.begin(); it != s_fonts.end(); ++it)
	{
		if (it->second)
		{
			f = it->second;
			delete f;
		}
	}

	s_fonts.clear();
}

void AssetManager::releaseAllShaders()
{
	sf::Shader* s;
	for (std::map<std::string, sf::Shader*>::iterator it = s_shaders.begin(); it != s_shaders.end(); ++it)
	{
		if (it->second)
		{
			s = it->second;
			delete s;
		}
	}

	s_shaders.clear();
}

void AssetManager::releaseAllSounds()
{
	sf::SoundBuffer* sb;
	for (std::map<std::string, sf::SoundBuffer*>::iterator it = s_soundBuffs.begin(); it != s_soundBuffs.end(); ++it)
	{
		if (it->second)
		{
			sb = it->second;
			delete sb;
		}
	}

	sf::Sound* s;
	for (std::map<std::string, sf::Sound*>::iterator it = s_sounds.begin(); it != s_sounds.end(); ++it)
	{
		if (it->second)
		{
			s = it->second;
			delete s;
		}
	}

	s_soundBuffs.clear();
	s_sounds.clear();
}

void AssetManager::releaseAllMusics()
{
	sf::Music* m;
	for (std::map<std::string, sf::Music*>::iterator it = s_musics.begin(); it != s_musics.end(); ++it)
	{
		if (it->second)
		{
			m = it->second;
			delete m;
		}
	}

	s_musics.clear();
}

void AssetManager::releaseAll()
{
	releaseAllImages();
	releaseAllTextures();
	releaseAllFonts();
	releaseAllShaders();
	releaseAllSounds();
	releaseAllMusics();
}
