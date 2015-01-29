#include "Scene/GameObject.h"
#include "Scene/Scene.h"
#include "Core.h"

GameObject::GameObject(const std::string& id) :
	//Id(this, &GameObject::setId, &GameObject::getId),
	//Order(this, &GameObject::setOrder, &GameObject::getOrder),
	//Active(this, &GameObject::setActive, &GameObject::isActive),
	m_id(id),
	m_parent(0),
	m_order(0),
	m_prefab(false),
	m_active(true),
	m_isDestroying(false)
{
}

GameObject::~GameObject()
{
	removeAllComponents();
	removeAllChildrens();
	processRemovingDelayedComponents();
	processRemoving();

	GameObject* o;
	for (List::iterator it = m_childrensToAdd.begin(); it != m_childrensToAdd.end(); it++)
	{
		o = *it;
		DELETE_OBJECT(o);
	}

	m_childrensToAdd.clear();
}

std::string GameObject::getId()
{
	return m_id;
}

void GameObject::setId(std::string id)
{
	m_id = id;
}

uint32 GameObject::getOrder()
{
	return m_order;
}

void GameObject::setOrder(uint32 order)
{
	m_order = order;
}

bool GameObject::isActive()
{
	return m_active;
}

void GameObject::setActive(bool flag)
{
	m_active = flag;
}

bool GameObject::isPrefab()
{
	return m_prefab;
}

GameObject* GameObject::getParent()
{
	return m_parent;
}

bool GameObject::isDestroying()
{
	return m_isDestroying;
}

void GameObject::duplicate(GameObject* from)
{
	if (from)
		from->onDuplicate(this);
}

bool GameObject::addComponent(Component* c)
{
	if (!c)
	{
		return false;
	}

	// it is possible to attach more than one Script components to the game object
	/*if (c->getType() != typeid(Script))
	{
		if (hasComponent(c->getType()))
		{
			return false;
		}
	}*/

	m_components.push_back(std::make_pair(c->getType(), c));
	c->setGameObject(this);

	return true;
}

void GameObject::removeComponent(Component* c, bool delayed)
{
	if (!c)
	{
		return;
	}

	for (Components::iterator it = m_components.begin(); it != m_components.end(); it++)
	{
		if (it->second == c)
		{
			if (delayed)
				m_componentsToDestroyDelayed.push_back(std::make_pair(c->getType(), c));
			else
			{
				c->setGameObject(0);
				DELETE_OBJECT(c);
				m_components.erase(it);
			}
			return;
		}
	}
}

void GameObject::removeComponent(std::type_index type, bool delayed)
{
	Component* c;
	for (Components::iterator it = m_components.begin(); it != m_components.end(); it++)
	{
		if (it->first == type)
		{
			c = it->second;
			if (delayed)
				m_componentsToDestroyDelayed.push_back(std::make_pair(c->getType(), c));
			else
			{
				c->setGameObject(0);
				DELETE_OBJECT(c);
				m_components.erase(it);
			}
			return;
		}
	}
}

void GameObject::removeAllComponents(bool delayed)
{
	if (delayed)
	{
		for (Components::iterator it = m_components.begin(); it != m_components.end(); it++)
			m_componentsToDestroyDelayed.push_back(*it);
	}
	else
	{
		Component* c;
		for (Components::iterator it = m_components.begin(); it != m_components.end(); it++)
		{
			c = it->second;
			c->setGameObject(0);
			DELETE_OBJECT(c);
		}

		m_components.clear();
	}
}

bool GameObject::hasComponent(Component* c)
{
	if (!c)
	{
		return false;
	}

	for (Components::iterator it = m_components.begin(); it != m_components.end(); it++)
		if (it->second == c)
			return true;

	return false;
}

bool GameObject::hasComponent(std::type_index type)
{
	for (Components::iterator it = m_components.begin(); it != m_components.end(); it++)
		if (it->first == type)
			return true;

	return false;
}

Component* GameObject::getComponent(std::type_index type)
{
	for (Components::iterator it = m_components.begin(); it != m_components.end(); it++)
		if (it->first == type)
			return it->second;

	return 0;
}

GameObject::Components GameObject::getComponentList()
{
	return m_components;
}

void GameObject::processRemovingDelayedComponents()
{
	Component* c;
	for (Components::iterator it = m_componentsToDestroyDelayed.begin(); it != m_componentsToDestroyDelayed.end(); it++)
	{
		if (std::find(m_components.begin(), m_components.end(), *it) == m_components.end())
			continue;
		c = it->second;
		m_components.remove(*it);
		c->setGameObject(0);
		DELETE_OBJECT(c);
	}

	m_componentsToDestroyDelayed.clear();
}

bool GameObject::isWaitingToRemoveDelayedComponent(Component* c)
{
	for (Components::iterator it = m_componentsToDestroyDelayed.begin(); it != m_componentsToDestroyDelayed.end(); it++)
		if (it->second == c)
			return true;

	return false;
}

bool GameObject::isWaitingToRemoveDelayedComponent(std::type_index type)
{
	for (Components::iterator it = m_componentsToDestroyDelayed.begin(); it != m_componentsToDestroyDelayed.end(); it++)
		if (it->first == type)
			return true;

	return false;
}

bool GameObject::addChildren(GameObject* o)
{
	if (!o)
	{
		IF_PRINT_WARNING(SCENE_DEBUG) << "attempt to add null game object(game object: " << m_id << ")" << std::endl;
		return false;
	}
	if (PTR_TYPEID(o) != typeid(GameObject))
	{
		IF_PRINT_WARNING(SCENE_DEBUG) << "attempt to add object of wrong type(game object: " << m_id << ")" << std::endl;
		return false;
	}
	if (hasChildren(o))
	{
		IF_PRINT_WARNING(SCENE_DEBUG) << "attempt to add game object that is already added(game object: " << m_id << ")"
			<< std::endl;
		return false;
	}
	if (isWaitingToAdd(o))
	{
		IF_PRINT_WARNING(SCENE_DEBUG) << "attempt to add game object that is already on the to-add list(game object: " << m_id << ")"
			<< std::endl;
		return false;
	}

	m_childrensToAdd.push_back(o);
	o->setParent(this);
	o->setDestroying(m_isDestroying);
	o->setPrefab(m_prefab);

	return true;
}

void GameObject::removeChildren(GameObject* o)
{
	if (!hasChildren(o))
	{
		return;
	}

	m_childrensToRemove.push_back(o);
	o->setDestroying(true);
	o->onDestroy();
}

void GameObject::removeChildren(const std::string& id)
{
	GameObject* o = getChildren(id);
	if (!o)
	{
		return;
	}

	m_childrensToRemove.push_back(o);
	o->setDestroying(true);
	o->onDestroy();
}

void GameObject::removeAllChildrens()
{
	GameObject* o;
	for (List::iterator it = m_childrens.begin(); it != m_childrens.end(); it++)
	{
		o = *it;
		m_childrensToRemove.push_back(o);
		o->setDestroying(true);
		o->onDestroy();
	}
}

bool GameObject::hasChildren(GameObject* o)
{
	for (List::iterator it = m_childrens.begin(); it != m_childrens.end(); it++)
		if (o == *it)
			return true;

	return false;
}

bool GameObject::hasChildren(const std::string& id)
{
	for (List::iterator it = m_childrens.begin(); it != m_childrens.end(); it++)
		if (id == (*it)->getId())
			return true;

	return false;
}

GameObject* GameObject::getChildren(const std::string& id)
{
	for (List::iterator it = m_childrens.begin(); it != m_childrens.end(); it++)
		if (id == (*it)->getId())
			return *it;

	return 0;
}

GameObject* GameObject::findChildren(const std::string& path)
{
	return findChildrenInPartOfPath(path, 0);
}

uint32 GameObject::getChildrenCount()
{
	return m_childrens.size();
}

void GameObject::processAdding()
{
	GameObject* o;
	for (List::iterator it = m_childrensToAdd.begin(); it != m_childrensToAdd.end(); it++)
	{
		o = *it;
		m_childrens.push_back(o);
		o->onCreate();
	}

	m_childrensToAdd.clear();
}

void GameObject::processRemoving()
{
	GameObject* o;
	for (List::iterator it = m_childrensToRemove.begin(); it != m_childrensToRemove.end(); it++)
	{
		o = *it;
		if (std::find(m_childrens.begin(), m_childrens.end(), o) == m_childrens.end())
			continue;
		m_childrens.remove(o);
		o->setParent(o);
		o->setPrefab(false);
		DELETE_OBJECT(o);
	}

	m_childrensToRemove.clear();
}

bool GameObject::isWaitingToAdd(GameObject* o)
{
	for (List::iterator it = m_childrensToAdd.begin(); it != m_childrensToAdd.end(); it++)
		if (o == *it)
			return true;

	return false;
}

bool GameObject::isWaitingToRemove(GameObject* o)
{
	for (List::iterator it = m_childrensToRemove.begin(); it != m_childrensToRemove.end(); it++)
		if (o == *it)
			return true;

	return false;
}

void GameObject::onCreate()
{
	for (Components::iterator it = m_components.begin(); it != m_components.end(); it++)
		it->second->onCreate();

	for (List::iterator it = m_childrens.begin(); it != m_childrens.end(); it++)
		(*it)->onCreate();
}

void GameObject::onDestroy()
{
	for (List::iterator it = m_childrens.begin(); it != m_childrens.end(); it++)
		(*it)->onDestroy();

	for (Components::iterator it = m_components.begin(); it != m_components.end(); it++)
		it->second->onDestroy();
}

void GameObject::onDuplicate(GameObject* dest)
{
	if (!dest)
	{
		return;
	}

	dest->setId(m_id);
	dest->setActive(m_active);
	dest->setOrder(m_order);

	Component* c;
	for (Components::iterator it = m_components.begin(); it != m_components.end(); it++)
	{
		std::type_index type = it->first;
		c = dest->getComponent(type);
		if (c)
			it->second->onDuplicate (c);
		else
		{
			c = Scene::buildComponent(type);
			it->second->onDuplicate(c);
			dest->addComponent(c);
		}
	}

	for (List::iterator it = m_childrens.begin(); it != m_childrens.end(); it++)
	{
		GameObject* o = new GameObject();
		dest->addChildren(o);
		(*it)->onDuplicate(o);
	}

	for (List::iterator it = m_childrensToAdd.begin(); it != m_childrensToAdd.end(); it++)
	{
		GameObject* o = new GameObject();
		dest->addChildren(o);
		(*it)->onDuplicate(o);
	}
}

void GameObject::onUpdate(const sf::Time& dt, const sf::Transform& trans, bool sort)
{
	if (m_active)
	{
		processRemovingDelayedComponents();

		Component* c;
		sf::Transform t = trans;
		for (Components::iterator it = m_components.begin(); it != m_components.end(); it++)
		{
			c = it->second;
			if (c->isActive())
			{
				c->onUpdate(dt);
				c->onTransform(t, t);
			}
		}

		processAdding();
		processRemoving();

		if (sort)
			m_childrens.sort([](GameObject* a, GameObject* b)->bool { return a->getOrder() > b->getOrder(); });

		for (List::iterator it = m_childrens.begin(); it != m_childrens.end(); it++)
			(*it)->onUpdate(dt, t, sort);
	}
}

void GameObject::onEvent(const sf::Event& event)
{
	if (m_active)
	{
		Component* c;
		for (Components::iterator it = m_components.begin(); it != m_components.end(); it++)
		{
			c = it->second;
			if (c->isActive())
				c->onEvent(event);
		}

		for (List::iterator it = m_childrens.begin(); it != m_childrens.end(); it++)
			(*it)->onEvent(event);
	}
}

void GameObject::onRender(sf::RenderTarget*& target)
{
	if (m_active)
	{
		List::iterator it = m_childrens.begin();
		while (it != m_childrens.end())
		{
			if ((*it)->getOrder() <= 0)
				break;
			(*it)->onRender(target);
			it++;
		}

		Component* c;
		for (Components::iterator it = m_components.begin(); it != m_components.end(); it++)
		{
			c = it->second;
			if (c->isActive())
				c->onRender(target);
		}

		while (it != m_childrens.end())
		{
			(*it)->onRender(target);
			it++;
		}
	}
}

void GameObject::onCollide(GameObject* other, bool beginOrEnd, b2Contact* contact)
{
	if (m_active)
	{
		Component* c;
		for (Components::iterator it = m_components.begin(); it != m_components.end(); it++)
		{
			c = it->second;
			if (c->isActive())
				c->onCollide(other, beginOrEnd, contact);
		}
	}
}

void GameObject::setParent(GameObject* parent)
{
	m_parent = parent;
}

void GameObject::setDestroying(bool flag)
{
	m_isDestroying = flag;
}

void GameObject::setPrefab(bool flag)
{
	m_prefab = flag;
}

GameObject* GameObject::findChildrenInPartOfPath(const std::string& path, uint32 from)
{
	unsigned int p = path.find('/', from);
    bool isRoot = from == 0 && p == 0 && p - from < 1;
    while(p - from < 1 && p != std::string::npos)
    {
        from++;
        p = path.find('/', from);
    }

	if (isRoot)
	{
		Scene::findGameObject(std::string(path, p + 1, -1));
	}
	else
	{
		std::string part = std::string(path, from, p);
		if (part.empty())
			return 0;
		else if (part == ".")
		{
			if (p == std::string::npos)
				return this;
			else
				return findChildrenInPartOfPath(path, p + 1);
		}
		else if (part == "..")
		{
			if (p == std::string::npos)
			{
				if (getParent())
					return getParent();
				else
					return 0;
			}
			else
			{
				if (getParent())
					return getParent()->findChildrenInPartOfPath(path, p + 1);
				else
					return Scene::findGameObject(std::string(path, p + 1, -1));
			}
		}
		else
		{
			GameObject* obj = getChildren(part);
			if (obj)
				return obj->findChildrenInPartOfPath(path, p + 1);
			else
				return 0;
		}
	}

	return 0;
}

template <class Archive>
void GameObject::save(Archive& ar, const unsigned int version) const
{
	ar & boost::serialization::make_nvp("active", m_active);
	ar & boost::serialization::make_nvp("id", m_id);
	ar & boost::serialization::make_nvp("order", m_order);

	uint32 componentsCount = m_components.size();
	ar & BOOST_SERIALIZATION_NVP(componentsCount);

	Component* component;
	for (Components::const_iterator it = m_components.begin(); it != m_components.end(); it++)
	{
		component = it->second;
		ar & BOOST_SERIALIZATION_NVP(component);
	}

	uint32 childrensCount = m_childrens.size() + m_childrensToAdd.size();
	ar & BOOST_SERIALIZATION_NVP(childrensCount);

	GameObject* gameobject;
	for (GameObject::List::const_iterator it = m_childrens.begin(); it != m_childrens.end(); it++)
	{
		gameobject = (*it);
		ar & BOOST_SERIALIZATION_NVP(gameobject);
	}
	for (GameObject::List::const_iterator it = m_childrensToAdd.begin(); it != m_childrensToAdd.end(); it++)
	{
		gameobject = (*it);
		ar & BOOST_SERIALIZATION_NVP(gameobject);
	}
}

template <class Archive>
void GameObject::load(Archive& ar, const unsigned int version)
{
	ar & boost::serialization::make_nvp("active", m_active);
	ar & boost::serialization::make_nvp("id", m_id);
	ar & boost::serialization::make_nvp("order", m_order);

	uint32 componentsCount;
	ar & BOOST_SERIALIZATION_NVP(componentsCount);
	m_components.clear();

	for (uint32 i = 0; i < componentsCount; i++)
	{
		Component* component;

		ar & BOOST_SERIALIZATION_NVP(component);
		if (!addComponent(component))
			DELETE_OBJECT(component);
	}

	uint32 childrensCount;
	ar & BOOST_SERIALIZATION_NVP(childrensCount);
	m_childrens.clear();

	for (uint32 i = 0; i < childrensCount; i++)
	{
		GameObject* gameobject;

		ar & BOOST_SERIALIZATION_NVP(gameobject);
		if (!addChildren(gameobject))
			DELETE_OBJECT(gameobject);
	}
}

DECLARE_BINARY_SAVE(GameObject)
DECLARE_BINARY_LOAD(GameObject)
