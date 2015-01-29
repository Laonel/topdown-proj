#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>

#include "Filesystem/Serialization.h"
#include "Filesystem/Archive.h"

#include "Utils.h"

class GameObject;

class Component
{
	friend class GameObject;

public:

	typedef Component* (*OnBuildComponentCallback)();

	Component();

	virtual ~Component();

	bool isActive();

	void setActive(bool value);

	GameObject* getOwner();

	inline std::type_index getType() { return PTR_TYPEID(this); }

protected:

	virtual void onCreate() { }
	virtual void onDestroy() { }
	virtual void onDuplicate(Component* dest);
	virtual void onUpdate(const sf::Time& dt) { }
	virtual void onEvent(const sf::Event& event) { }
	virtual void onRender(sf::RenderTarget*& target) { }
	virtual void onTransform(const sf::Transform& inTrans, sf::Transform& outTrans) { }
	virtual void onCollide(GameObject* other, bool beginOrEnd, b2Contact* contact) { }

private:

	void setGameObject(GameObject* object);

private:

	bool m_active;

	GameObject* m_owner;

private:

	friend class boost::serialization::access;

	template <class Archive>
	void save(Archive& ar, const unsigned int version) const;

	template <class Archive>
	void load(Archive& ar, const unsigned int version);

	BOOST_SERIALIZATION_SPLIT_MEMBER()
};

BOOST_CLASS_VERSION(Component, 1)

BOOST_CLASS_EXPORT_KEY(Component)

#endif
