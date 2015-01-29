#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include <SFML/Graphics.hpp>

#include "Scene/Component.h"

#include "Utils.h"

#include "Filesystem/Archive.h"

class GameObject
{
	friend class Scene;

public:

	typedef std::list<GameObject*> List;
	typedef std::pair<std::type_index, Component*> ComponentPair;
	typedef std::list<ComponentPair> Components;

	inline static GameObject* buildGameObject(const std::string& name) { return new GameObject(name); }

	GameObject(const std::string& id = "");

	~GameObject();

	std::string getId();

	void setId(std::string id);

	uint32 getOrder();

	void setOrder(uint32 order);

	bool isActive();

	void setActive(bool flag);

	bool isPrefab();

	GameObject* getParent();

	bool isDestroying();

	void duplicate(GameObject* from);

	bool addComponent(Component* c);

	void removeComponent(Component* c, bool delayed = false);
	void removeComponent(std::type_index type, bool delayed = false);
	template <typename T>
	void removeComponent(bool delayed = false);
	void removeAllComponents(bool delayed = false);

	bool hasComponent(Component* c);
	template <typename T>
	bool hasComponent();
	bool hasComponent(std::type_index type);

	Component* getComponent(std::type_index type);
	template <typename T>
	T* getComponent();

	Components getComponentList();

	void processRemovingDelayedComponents();

	bool isWaitingToRemoveDelayedComponent(Component* c);
	bool isWaitingToRemoveDelayedComponent(std::type_index type);
	template <typename T>
	bool isWaitingToRemoveDelayedComponent();

	bool addChildren(GameObject* o);

	void removeChildren(GameObject* o);
	void removeChildren(const std::string& id);
	void removeAllChildrens();

	bool hasChildren(GameObject* o);
	bool hasChildren(const std::string& id);

	GameObject* getChildren(const std::string& id);
	GameObject* findChildren(const std::string& path);

	uint32 getChildrenCount();

	void processAdding();
	void processRemoving();

	bool isWaitingToAdd(GameObject* o);
	bool isWaitingToRemove(GameObject* o);

protected:

	void onCreate();
	void onDestroy();
	void onDuplicate(GameObject* dest);
	void onUpdate(const sf::Time& dt, const sf::Transform& trans, bool sort = false);
	void onEvent(const sf::Event& event);
	void onRender(sf::RenderTarget*& target);
	void onCollide(GameObject* other, bool beginOrEnd, b2Contact* contact);

private:

	void setParent(GameObject* parent);

	void setDestroying(bool flag);

	void setPrefab(bool flag);

	GameObject* findChildrenInPartOfPath(const std::string& path, uint32 from);

private:

	GameObject* m_parent;

	std::string m_id;
	uint32 m_order;

	bool m_active;
	bool m_prefab;
	bool m_isDestroying;

	Components m_components;
	Components m_componentsToDestroyDelayed;

	List m_childrens;
	List m_childrensToAdd;
	List m_childrensToRemove;

private:

	friend class boost::serialization::access;

	template <class Archive>
	void save(Archive& ar, const unsigned int version) const;

	template <class Archive>
	void load(Archive& ar, const unsigned int version);

	BOOST_SERIALIZATION_SPLIT_MEMBER()

};

template <typename T>
void GameObject::removeComponent(bool delayed)
{
	removeComponent(typeid(T));
}

template <typename T>
bool GameObject::hasComponent()
{
	hasComponent(typeid(T));
}

template <typename T>
T* GameObject::getComponent()
{
	return (T*)(getComponent(typeid(T)));
}

template <typename T>
bool GameObject::isWaitingToRemoveDelayedComponent()
{
	return isWaitingToRemoveDelayedComponent(typeid(T));
}

#endif
