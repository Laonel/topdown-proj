#include "Scene/Material.h"
#include "Filesystem/Assets/AssetManager.h"

Material::Material()
{
}

Material::~Material()
{
	clear();
}

float Material::getFloat(const std::string& name)
{
	return count(name) > 0 ? access(name)[0] : 0.f;
}

sf::Vector2f Material::getVec2(const std::string& name)
{
	return count(name) > 1 ? sf::Vector2f(access(name)[0], access(name)[1]) : sf::Vector2f();
}

sf::Vector3f Material::getVec3(const std::string& name)
{
	return count(name) > 2 ? sf::Vector3f(access(name)[0], access(name)[1], access(name)[2]) : sf::Vector3f();
}

sf::Color Material::getColor(const std::string& name)
{
	if (count(name) < 4)
		return sf::Color::Transparent;

	std::vector<float>& v = access(name);
	return sf::Color(
		uint8(clamp(v[0], 0.f, 1.f) * 255.f),
		uint8(clamp(v[1], 0.f, 1.f) * 255.f),
		uint8(clamp(v[2], 0.f, 1.f) * 255.f),
		uint8(clamp(v[3], 0.f, 1.f) * 255.f)
	);
}

sf::Transform Material::getTransform(const std::string& name)
{
	if (count(name) < 16)
		return sf::Transform::Identity;

	sf::Transform t;
	float* m = (float*)(t.getMatrix());
	std::vector<float> v = access(name);

	for (uint32 i = 0; i < 16; ++i)
		m[i] = v[i];

	return t;
}

float* Material::getRaw(const std::string& name)
{
	return has(name) ? access(name).data() : 0;
}

sf::Texture* Material::getTexture(const std::string& name)
{
	return hasTexture(name) ? m_textures[name] : 0;
}

void Material::set(const std::string& name, float x)
{
	removeTexture(name);

	std::vector<float>& v = access(name);
	v.resize(1);
	v.clear();
	v.push_back(x);
}

void Material::set(const std::string& name, float x, float y)
{
	removeTexture(name);

	std::vector<float>& v = access(name);
	v.resize(2);
	v.clear();
	v.push_back(x);
	v.push_back(y);
}

void Material::set(const std::string& name, float x, float y, float z)
{
	removeTexture(name);

	std::vector<float>& v = access(name);
	v.resize(3);
	v.clear();
	v.push_back(x);
	v.push_back(y);
	v.push_back(z);
}

void Material::set(const std::string& name, float x, float y, float z, float w)
{
	removeTexture(name);

	std::vector<float>& v = access(name);
	v.resize(4);
	v.clear();
	v.push_back(x);
	v.push_back(y);
	v.push_back(z);
	v.push_back(w);
}

void Material::set(const std::string& name, const sf::Vector2f& vec)
{
	removeTexture(name);

	std::vector<float>& v = access(name);
	v.resize(2);
	v.clear();
	v.push_back(vec.x);
	v.push_back(vec.y);
}

void Material::set(const std::string& name, const sf::Vector3f& vec)
{
	removeTexture(name);

	std::vector<float>& v = access(name);
	v.resize(3);
	v.clear();
	v.push_back(vec.x);
	v.push_back(vec.y);
	v.push_back(vec.z);
}

void Material::set(const std::string& name, const sf::Color& color)
{
	const float c = 1.f / 255.f;
	removeTexture(name);

	std::vector<float>& v = access(name);
	v.resize(4);
	v.clear();
	v.push_back((float)(color.r) * c);
	v.push_back((float)(color.g) * c);
	v.push_back((float)(color.b) * c);
	v.push_back((float)(color.a) * c);
}

void Material::set(const std::string& name, const sf::Transform& trans)
{
	removeTexture(name);

	std::vector<float>& v = access(name);
	v.resize(16);
	v.clear();

	const float* m = trans.getMatrix();
	for (uint32 i = 0; i < 16; ++i)
		v.push_back(m[i]);
}

void Material::set(const std::string& name, const sf::Texture* tex)
{
	remove(name);

	if(tex)
		m_textures[name] = (sf::Texture*)(tex);
}

bool Material::has(const std::string& name)
{
	return static_cast<bool>(m_properties.count(name));
}

bool Material::hasTexture(const std::string& name)
{
	return static_cast<bool>(m_textures.count(name));
}

void Material::remove(const std::string& name)
{
	if (has(name))
		m_properties.erase(name);
}

void Material::removeTexture(const std::string& name)
{
	if (hasTexture(name))
		m_textures.erase(name);
}

std::vector<float>& Material::access(const std::string& name)
{
	return m_properties[name];
}

int Material::count(const std::string& name)
{
	return m_properties.count(name) ? m_properties.size() : -1;
}

void Material::clear()
{
	m_properties.clear();
	m_textures.clear();
}

void Material::apply(sf::Shader* shader, bool propsValidation)
{
	if (!shader)
		return;

	std::vector<std::string>* u = propsValidation ?
		AssetManager::getShaderUniforms(AssetManager::findShader(shader)) :
		0;

	std::string n;
	for (Properties::iterator it = m_properties.begin(); it != m_properties.end(); ++it)
	{
		n.assign(it->first);
		if (u && std::find(u->begin(), u->end(), n) == u->end())
			continue;

		std::vector<float>& v = it->second;
		int s = v.size();
		if (s == 1)
			shader->setParameter(n, v[0]);
		else if (s == 2)
			shader->setParameter(n, v[0], v[1]);
		else if (s == 3)
			shader->setParameter(n, v[0], v[1], v[2]);
		else if (s == 4)
			shader->setParameter(n, v[0], v[1], v[2], v[3]);
		else if (s == 16)
		{
			sf::Transform t;
			float* m = (float*)(t.getMatrix());
			for (uint32 i = 0; i < 16; ++i)
				m[i] = v[i];
			shader->setParameter(n, t);
		}
	}

	for (Textures::iterator it = m_textures.begin(); it != m_textures.end(); ++it)
		shader->setParameter(it->first, *it->second);
}

void Material::copyFrom(Material& mat)
{
	m_properties.clear();
	m_properties = mat.m_properties;
	m_textures.clear();
	m_textures = mat.m_textures;
}
