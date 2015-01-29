#include "Scene/Component.h"
#include "Scene/GameObject.h"
#include "Core.h"

Component::Component() :
	m_active(true),
	m_owner(0)
	//Active(this, &Component::setActive, &Component::isActive),
	//Owner(this, 0, &Component::getOwner)
{
}

Component::~Component()
{
	if (m_owner)
		m_owner->removeComponent(this);
}

bool Component::isActive()
{
	return m_active;
}

void Component::setActive(bool active)
{
	m_active = active;
}

GameObject* Component::getOwner()
{
	return m_owner;
}

void Component::onDuplicate(Component* dest)
{
	if (!dest)
		return;

	dest->setActive(true);
}

void Component::setGameObject(GameObject* object)
{
	m_owner = object;
}

template <class Archive>
void Component::save(Archive& ar, const unsigned int version) const
{
	ar & boost::serialization::make_nvp("active", m_active);
}

template <class Archive>
void Component::load(Archive& ar, const unsigned int version)
{
	ar & boost::serialization::make_nvp("active", m_active);
}

DECLARE_BINARY_SAVE(Component)
DECLARE_BINARY_LOAD(Component)

BOOST_CLASS_EXPORT_IMPLEMENT(Component)
