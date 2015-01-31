#include "Scene/Components/Transform.h"
#include "Scene/GameObject.h"

Transform::Transform() :
	m_position(sf::Vector2f(0.f, 0.f)),
	m_rotation(0.f),
	m_scale(sf::Vector2f(0.f, 0.f)),
	m_mode(Hierarchy)
{
}

Transform::~Transform()
{
}

sf::Vector2f Transform::getPosition()
{
	return m_position;
}

void Transform::setPosition(sf::Vector2f pos)
{
	m_position = pos;
}

float Transform::getRotation()
{
	return m_rotation;
}

void Transform::setRotation(float rot)
{
	m_rotation = rot;
}

sf::Vector2f Transform::getScale()
{
	return m_scale;
}

void Transform::setScale(sf::Vector2f scale)
{
	m_scale = scale;
}

const sf::Transform& Transform::getTransform()
{
	return m_transform;
}

const sf::Transform& Transform::getGlobalTransform()
{
	return m_globalTransform;
}

void Transform::recomputeTransform()
{
	if (getOwner())
	{
		if (getOwner()->getParent())
		{
			Transform* trans = getOwner()->getParent()->getComponent<Transform>();
			if (trans)
				trans->recomputeTransform();
		}
	}

	sf::Transform t;
	onTransform(t, t);
}

void Transform::onDuplicate(Component* dest)
{
	if (!dest)
		return;

	Component::onDuplicate(dest);
	if (PTR_TYPEID(dest) != typeid(Transform))
		return;

	Transform* c = (Transform*)(dest);
	c->setPosition(getPosition());
	c->setRotation(getRotation());
	c->setScale(getScale());
	c->setMode(getMode());
}

void Transform::onTransform(const sf::Transform& inTrans, sf::Transform& outTrans)
{
	sf::Transform t;
	if (m_mode == Hierarchy)
	{
		t.translate(m_position);
		t.rotate(m_rotation);
		t.scale(m_scale);
		outTrans = inTrans * t;
	}
	else if (m_mode == Parent)
	{
		outTrans = inTrans;
	}
	else
	{
		t.translate(m_position);
		t.rotate(m_rotation);
		t.scale(m_scale);
		outTrans = t;
	}

	m_transform = t;
	m_globalTransform = outTrans;
}

template <class Archive>
void Transform::save(Archive& ar, const unsigned int version) const
{
	ar & boost::serialization::base_object<const Component>(*this);

	ar & boost::serialization::make_nvp("position", m_position);
	ar & boost::serialization::make_nvp("rotation", m_rotation);
	ar & boost::serialization::make_nvp("scale", m_scale);
	ar & boost::serialization::make_nvp("mode", m_mode);
}

template <class Archive>
void Transform::load(Archive& ar, const unsigned int version)
{
	ar & boost::serialization::base_object<Component>(*this);

	ar & boost::serialization::make_nvp("position", m_position);
	ar & boost::serialization::make_nvp("rotation", m_rotation);
	ar & boost::serialization::make_nvp("scale", m_scale);
	ar & boost::serialization::make_nvp("mode", m_mode);
}

DECLARE_BINARY_SAVE(Transform)
DECLARE_BINARY_LOAD(Transform)

BOOST_CLASS_EXPORT_IMPLEMENT(Transform)
