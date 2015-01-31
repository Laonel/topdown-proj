#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Utils.h"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Shader.hpp>

class Material
{
public:

	typedef std::map<std::string, std::vector<float>> Properties;
	typedef std::map<std::string, sf::Texture*> Textures;

	Material();

	~Material();

	float getFloat(const std::string& name);
	sf::Vector2f getVec2(const std::string& name);
	sf::Vector3f getVec3(const std::string& name);
	sf::Color getColor(const std::string& name);
	sf::Transform getTransform(const std::string& name);
	float* getRaw(const std::string& name);
	sf::Texture* getTexture(const std::string& name);

	void set(const std::string& name, float x);
	void set(const std::string& name, float x, float y);
	void set(const std::string& name, float x, float y, float z);
	void set(const std::string& name, float x, float y, float z, float w);
	void set(const std::string& name, const sf::Vector2f& vec);
	void set(const std::string& name, const sf::Vector3f& vec);
	void set(const std::string& name, const sf::Color& color);
	void set(const std::string& name, const sf::Transform& trans);
	void set(const std::string& name, const sf::Texture* tex);

	bool has(const std::string& name);
	bool hasTexture(const std::string& name);

	void remove(const std::string& name);
	void removeTexture(const std::string& name);

	std::vector<float>& access(const std::string& name);

	int count(const std::string& name);

	void clear();

	void apply(sf::Shader* shader, bool propsValidation);

	void copyFrom(Material& mat);

private:

	Properties m_properties;
	Textures m_textures;
	
};

#endif
