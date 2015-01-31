#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Scene/Component.h"

#include <SFML/Graphics/Transform.hpp>

class Transform : public Component
{
public:

	inline static Component* onBuildComponent() { return new Transform(); }

	enum ModeType
	{
		Hierarchy,
		Parent,
		Global
	};

	Transform();

	virtual ~Transform();

	sf::Vector2f getPosition();

	void setPosition(sf::Vector2f pos);

	float getRotation();

	void setRotation(float rot);

	sf::Vector2f getScale();

	void setScale(sf::Vector2f scale);

	inline ModeType getMode() { return m_mode; }

	inline void setMode(ModeType mode) { m_mode = mode; }

	const sf::Transform& getTransform();

	const sf::Transform& getGlobalTransform();

	void recomputeTransform();

protected:

	virtual void onDuplicate(Component* dest);
	virtual void onTransform(const sf::Transform& inTrans, sf::Transform& outTrans);

private:

	sf::Vector2f m_position;
	float m_rotation;
	sf::Vector2f m_scale;
	ModeType m_mode;

	sf::Transform m_transform;
	sf::Transform m_globalTransform;

private:

	friend class boost::serialization::access;

	template <class Archive>
	void save(Archive& ar, const unsigned int version) const;

	template <class Archive>
	void load(Archive& ar, const unsigned int version);

	BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(Transform, 1)

BOOST_CLASS_EXPORT_KEY(Transform)

#endif
