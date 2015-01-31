#ifndef _SPRITE_RENDERER_H_
#define _SPRITE_RENDERER_H_

#include "Scene/Component.h"
#include "Scene/Material.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

class SpriteRenderer : public Component
{
public:

	inline static Component* onBuildComponent() { return new SpriteRenderer(); }

	SpriteRenderer();

	virtual ~SpriteRenderer();

	sf::RectangleShape* getRenderer();

	void setTexture(sf::Texture* tex);

	sf::Texture* getTexture() const;

	void setSize(sf::Vector2f size);

	sf::Vector2f getSize();

	void setOrigin(sf::Vector2f origin);

	sf::Vector2f getOrigin();

	void setOriginPercent(sf::Vector2f origin);

	sf::Vector2f getOriginPercent() const;

	void setColor(sf::Color color);

	sf::Color getColor();

	void setRenderStates(sf::RenderStates states);

	sf::RenderStates getRenderStates();

	void setMaterial(Material m);

	Material getMaterial();

	void setMaterialValidation(bool value);

	bool getMaterialValidation();

protected:

	virtual void onDuplicate(Component* dest);
	virtual void onRender(sf::RenderTarget*& target);
	virtual void onTransform(const sf::Transform& inTrans, sf::Transform& outTrans);

private:

	sf::RectangleShape* m_shape;
	sf::RenderStates m_states;

	Material m_material;
	bool m_materialValidation;

private:

	friend class boost::serialization::access;

	template <class Archive>
	void save(Archive& ar, const unsigned int version) const;

	template <class Archive>
	void load(Archive& ar, const unsigned int version);

	BOOST_SERIALIZATION_SPLIT_MEMBER()
};

BOOST_CLASS_VERSION(SpriteRenderer, 1)

BOOST_CLASS_EXPORT_KEY(SpriteRenderer)

#endif
