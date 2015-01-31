#include "Scene/Components/Rendering/SpriteRenderer.h"
#include "Filesystem/Assets/AssetManager.h"

SpriteRenderer::SpriteRenderer() :
	m_states(sf::RenderStates::Default),
	m_materialValidation(false)
{
	m_shape = new sf::RectangleShape();
	setTexture(0);
}

SpriteRenderer::~SpriteRenderer()
{
	DELETE_OBJECT(m_shape);
}

sf::RectangleShape* SpriteRenderer::getRenderer()
{
	return m_shape;
}

void SpriteRenderer::setTexture(sf::Texture* tex)
{
	sf::Texture* t = tex ? tex : AssetManager::getDefaultTexture();
	m_shape->setTexture(t);
 	if (t)
	{
		sf::Vector2u s = t->getSize();
		m_shape->setTextureRect(sf::IntRect(0, 0, s.x, s.y));
	}
}

sf::Texture* SpriteRenderer::getTexture() const
{
	sf::Texture* tex = (sf::Texture*)(m_shape->getTexture());
	return tex == AssetManager::getDefaultTexture() ? 0 : tex;
}

void SpriteRenderer::setSize(sf::Vector2f size)
{
	sf::Texture* t = getTexture();
	if (size.x < 0.f)
		size.x = t ? t->getSize().x : 0.f;
	if (size.y < 0.f)
		size.y = t ? t->getSize().y : 0.f;

	m_shape->setSize(size);
}

sf::Vector2f SpriteRenderer::getSize()
{
	return m_shape->getSize();
}

void SpriteRenderer::setOrigin(sf::Vector2f origin)
{
	m_shape->setOrigin(origin);
}

sf::Vector2f SpriteRenderer::getOrigin()
{
	return m_shape->getOrigin();
}

void SpriteRenderer::setOriginPercent(sf::Vector2f origin)
{
	sf::Vector2f s = getSize();
	setOrigin(sf::Vector2f(s.x * origin.x, s.y * origin.y));
}

sf::Vector2f SpriteRenderer::getOriginPercent() const
{
	sf::Vector2f o = m_shape->getOrigin();
	sf::Vector2f s = m_shape->getSize();
	o.x = s.x > 0.f ? o.x / s.x : 0.f;
	o.y = s.y > 0.f ? o.y / s.y : 0.f;
	return o;
}

void SpriteRenderer::setColor(sf::Color color)
{
	m_shape->setFillColor(color);
}

sf::Color SpriteRenderer::getColor()
{
	return m_shape->getFillColor();
}

void SpriteRenderer::setRenderStates(sf::RenderStates states)
{
	m_states = states;
}

sf::RenderStates SpriteRenderer::getRenderStates()
{
	return m_states;
}

void SpriteRenderer::setMaterial(Material m)
{
	m_material.copyFrom(m);
}

Material SpriteRenderer::getMaterial()
{
	return m_material;
}

void SpriteRenderer::setMaterialValidation(bool value)
{
	m_materialValidation = value;
}

bool SpriteRenderer::getMaterialValidation()
{
	return m_materialValidation;
}

void SpriteRenderer::onDuplicate(Component* dest)
{
	if (!dest)
		return;

	Component::onDuplicate(dest);
	if (PTR_TYPEID(dest) != typeid(SpriteRenderer))
		return;

	SpriteRenderer* r = (SpriteRenderer*)(dest);
	r->setMaterial(getMaterial());
	r->setMaterialValidation(getMaterialValidation());
	r->setTexture(getTexture());
	r->setSize(getSize());
	r->setOrigin(getOrigin());
	r->setRenderStates(getRenderStates());
}

void SpriteRenderer::onRender(sf::RenderTarget*& target)
{
	if (m_states.shader)
		m_material.apply((sf::Shader*)(m_states.shader), m_materialValidation);

	target->draw(*m_shape, m_states);
}

void SpriteRenderer::onTransform(const sf::Transform& inTrans, sf::Transform& outTrans)
{
	m_states.transform = inTrans;
}

template <class Archive>
void SpriteRenderer::save(Archive& ar, const unsigned int version) const
{
	ar & boost::serialization::base_object<const Component>(*this);

	std::string texId = AssetManager::findTexture(getTexture());
	ar & BOOST_SERIALIZATION_NVP(texId);

	sf::Vector2f s = m_shape->getSize();
	ar & boost::serialization::make_nvp("size", s);

	sf::Vector2f o = m_shape->getOrigin();
	ar & boost::serialization::make_nvp("origin", o);

	sf::Vector2f op = getOriginPercent();
	ar & boost::serialization::make_nvp("originPercent", op);

	sf::Color c = m_shape->getFillColor();
	ar & boost::serialization::make_nvp("color", c);
}

template <class Archive>
void SpriteRenderer::load(Archive& ar, const unsigned int version)
{
	ar & boost::serialization::base_object<Component>(*this);

	std::string texId;
	ar & BOOST_SERIALIZATION_NVP(texId);
	sf::Texture* tex = AssetManager::getTexture(texId);
	if(tex)
		setTexture(tex);

	sf::Vector2f s;
	ar & boost::serialization::make_nvp("size", s);
	setSize(s);

	sf::Vector2f o;
	ar & boost::serialization::make_nvp("origin", o);
	setOrigin(o);

	sf::Vector2f op;
	ar & boost::serialization::make_nvp("originPercent", op);
	setOriginPercent(op);

	sf::Color c;
	ar & boost::serialization::make_nvp("color", c);
	setColor(c);
}

DECLARE_BINARY_SAVE(SpriteRenderer)
DECLARE_BINARY_LOAD(SpriteRenderer)

BOOST_CLASS_EXPORT_IMPLEMENT(SpriteRenderer)
