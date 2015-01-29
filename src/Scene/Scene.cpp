#include "Scene/Scene.h"
#include "Core.h"

bool SCENE_DEBUG = true;

std::map<std::string, Scene::ComponentFactoryData> Scene::m_componentFactory;

GameObject::List Scene::m_prefabs;
GameObject::List Scene::m_gameObjects;
GameObject::List Scene::m_gameObjectsToCreate;
GameObject::List Scene::m_gameObjectsToDestroy;

std::string Scene::s_sceneFileExtension = ".scn";

void Scene::registerComponentFactory(const std::string& id, std::type_index type, Component::OnBuildComponentCallback builder)
{
	if (id.empty() || m_componentFactory.count(id) || !builder)
		return;

	ComponentFactoryData d;
	d.type = type;
	d.builder = builder;
	m_componentFactory[id] = d;
}

void Scene::unregisterComponentFactory(const std::string& id)
{
	if (m_componentFactory.count(id))
		m_componentFactory.erase(id);
}

void Scene::unregisterComponentFactory(std::type_index type)
{
	for (std::map<std::string, ComponentFactoryData>::iterator it = m_componentFactory.begin();
		it != m_componentFactory.end();
		++it)
	{
		if (it->second.type == type)
		{
			m_componentFactory.erase(it);
			return;
		}
	}
}

void Scene::unregisterComponentFactory(Component::OnBuildComponentCallback builder)
{
	for (std::map<std::string, ComponentFactoryData>::iterator it = m_componentFactory.begin();
		it != m_componentFactory.end();
		++it)
	{
		if (it->second.builder == builder)
		{
			m_componentFactory.erase(it);
			return;
		}
	}
}

void Scene::unregisterAllComponentFactories()
{
	m_componentFactory.clear();
}

std::type_index* Scene::findComponentFactoryType(const std::string& id)
{
	if (m_componentFactory.count(id))
		return &(m_componentFactory[id].type);

	return 0;
}

std::type_index* Scene::findComponentFactoryType(Component::OnBuildComponentCallback builder)
{
	for (std::map<std::string, ComponentFactoryData>::iterator it = m_componentFactory.begin();
		it != m_componentFactory.end();
		++it)
		if (it->second.builder == builder)
			return &(it->second.type);

	return 0;
}

std::string Scene::findComponentFactoryId(std::type_index type)
{
	for (std::map<std::string, ComponentFactoryData>::iterator it = m_componentFactory.begin();
		it != m_componentFactory.end();
		++it)
		if (it->second.type == type)
			return it->first;

	return std::string();
}

std::string Scene::findComponentFactoryId(Component::OnBuildComponentCallback builder)
{
	for (std::map<std::string, ComponentFactoryData>::iterator it = m_componentFactory.begin();
		it != m_componentFactory.end();
		++it)
		if (it->second.builder == builder)
			return it->first;

	return std::string();
}

Component::OnBuildComponentCallback Scene::findComponentFactoryBuilder(const std::string& id)
{
	if (m_componentFactory.count(id))
		return m_componentFactory[id].builder;

	return 0;
}

Component::OnBuildComponentCallback Scene::findComponentFactoryBuilder(std::type_index type)
{
	for (std::map<std::string, ComponentFactoryData>::iterator it = m_componentFactory.begin();
		it != m_componentFactory.end();
		++it)
		if (it->second.type == type)
			return it->second.builder;

	return 0;
}

Component* Scene::buildComponent(const std::string& id)
{
	if (m_componentFactory.count(id))
		return m_componentFactory[id].builder();

	return 0;
}

Component* Scene::buildComponent(std::type_index type)
{
	for (std::map<std::string, ComponentFactoryData>::iterator it = m_componentFactory.begin();
		it != m_componentFactory.end();
		++it)
		if (it->second.type == type)
			return it->second.builder();

	return 0;
}

bool Scene::init()
{/*
	registerComponentFactory("Transform", typeid(Transform), Transform::onBuildComponent);
	registerComponentFactory("SpriteRenderer", typeid(SpriteRenderer), SpriteRenderer::onBuildComponent);
	registerComponentFactory("Script", typeid(Script), Script::onBuildComponent);
	registerComponentFactory("Camera", typeid(Camera), Camera::onBuildComponent);
	registerComponentFactory("Body", typeid(Body), Body::onBuildComponent);
	registerComponentFactory("RevoluteJoint", typeid(RevoluteJoint), RevoluteJoint::onBuildComponent);
	registerComponentFactory("PrismaticJoint", typeid(PrismaticJoint), PrismaticJoint::onBuildComponent);
	registerComponentFactory("DistanceJoint", typeid(DistanceJoint), DistanceJoint::onBuildComponent);
	registerComponentFactory("Tilemap", typeid(Tilemap), Tilemap::onBuildComponent);
*/
	return true;
}

void Scene::shutdown()
{
	removeAllGameObjects();
	processRemoving();

	GameObject* obj;
	for (GameObject::List::iterator it = m_gameObjectsToCreate.begin(); it != m_gameObjectsToCreate.end(); ++it)
	{
		obj = *it;
		DELETE_OBJECT(obj);
	}

	m_gameObjectsToCreate.clear();
	
	unregisterAllComponentFactories();
}

bool Scene::saveScene(const std::string& path)
{	
	// Check if the file has correct extension
	boost::filesystem::path p(path.c_str());
	if (p.extension().string() != s_sceneFileExtension)
		p.replace_extension(s_sceneFileExtension);

	// Standard output file stream used with binary archive
	std::ofstream ofs(path, std::fstream::binary | std::fstream::out);
	if (!ofs.is_open())
	{
		IF_PRINT_WARNING(SCENE_DEBUG) << "Can not open the file: " << path << std::endl;
		return false;
	}

	// Binary archive
	{
		eos::portable_oarchive ar(ofs);

		std::string name = p.filename().string();

		// save filename to the archive so it can be checked when loading
		ar << name;

		// save special key to the archive to check if the archive is original before loading
		//ar << secureKey;

		// save prefabs to the archive
		uint32 prefabsCount = m_prefabs.size();
		ar << prefabsCount;

		GameObject* go;
		for (GameObject::List::iterator it = m_prefabs.begin(); it != m_prefabs.end(); it++)
		{
			go = (*it);
			ar << go;
		}

		// save gameobjects to the archive
		uint32 goCount = m_gameObjects.size() + m_gameObjectsToCreate.size();
		ar << goCount;
		for (GameObject::List::iterator it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
		{
			go = (*it);
			ar << go;
		}
		for (GameObject::List::iterator it = m_gameObjectsToCreate.begin(); it != m_gameObjectsToCreate.end(); it++)
		{
			go = (*it);
			ar << go;
		}
	}

	ofs.close();
	return true;
}

bool Scene::loadScene(const std::string& path)
{
	// Check if the file has correct extension
	boost::filesystem::path p(path.c_str());
	if (p.extension().string() != s_sceneFileExtension)
	{
		IF_PRINT_WARNING(SCENE_DEBUG) << "Archive has wrong extension" << std::endl;
		return false;
	}

	// Standard input file stream used with binary archive
	std::ifstream ifs(path, std::fstream::binary | std::fstream::in);
	if (!ifs.is_open())
	{
		IF_PRINT_WARNING(SCENE_DEBUG) << "Can not open the file: " << path << std::endl;
		return false;
	}

	if (ifs.bad() || ifs.fail())
		return false;

	// Binary archive
	{
		eos::portable_iarchive ar(ifs);

		std::string name;
		ar >> name;

		if (p.filename() != name)
		{
			IF_PRINT_WARNING(SCENE_DEBUG) << "Filename doesn't match with the original" << std::endl;
			return false;
		}

		/*uint8 readKey[32];
		ar >> readKey;
		if (readKey != secureKey)
		{
			IF_PRINT_WARNING(SCENE_DEBUG) << "Secure keys doesn't match" << std::endl;
			return false;
		}*/

		uint32 prefabCount;
		ar >> prefabCount;

		for (uint32 i = 0; i < prefabCount; i++)
		{
			GameObject* go;

			ar >> go;
			if (!addGameObject(go, true))
				DELETE_OBJECT(go);
		}

		uint32 goCount;
		ar >> goCount;

		for (uint32 i = 0; i < goCount; i++)
		{
			GameObject* go;

			ar >> go;
			if (!addGameObject(go))
				DELETE_OBJECT(go);
		}
	}

	ifs.close();
	return true;
}

void Scene::removeScene()
{
	removeAllGameObjects(true);
	removeAllGameObjects(false);
}

bool Scene::addGameObject(GameObject* obj, bool prefab)
{
	if (!obj || (typeid(obj) != typeid(GameObject*)) || hasGameObject(obj, prefab) || isWaitingToAdd(obj))
		return false;

	GameObject::List& cgo = prefab ? m_prefabs : m_gameObjectsToCreate;
	cgo.push_back(obj);

	obj->setPrefab(prefab);
	obj->setDestroying(false);

	return true;
}

void Scene::removeGameObject(GameObject* obj, bool prefab)
{
	if (!obj)
	{
		IF_PRINT_WARNING(SCENE_DEBUG) << "attempt to remove null game object: " << obj->getId() << std::endl;
		return;
	}

	if (!hasGameObject(obj, prefab))
	{
		IF_PRINT_WARNING(SCENE_DEBUG) << "attempt to remove game object that doesn't exist: " << obj->getId() << std::endl;
		return;
	}

	if (prefab)
	{
		m_prefabs.remove(obj);
		obj->setPrefab(false);
		DELETE_OBJECT(obj);
	}
	else
	{
		m_gameObjectsToDestroy.push_back(obj);
		if (!prefab)
		{
			obj->setDestroying(true);
			obj->onDestroy();
		}
	}
}

void Scene::removeGameObject(const std::string& id, bool prefab)
{
	GameObject* o = getGameObject(id, prefab);
	if (!o)
	{
		IF_PRINT_WARNING(SCENE_DEBUG) << "attempt to remove null game object: " << o->getId() << std::endl;
		return;
	}

	if (prefab)
	{
		m_prefabs.remove(o);
		o->setPrefab(false);
		DELETE_OBJECT(o);
	}
	else
	{
		m_gameObjectsToDestroy.push_back(o);
		if (!prefab)
		{
			o->setDestroying(true);
			o->onDestroy();
		}
	}
}

void Scene::removeAllGameObjects(bool prefab)
{
	GameObject* obj;
	if (prefab)
	{
		for (GameObject::List::iterator it = m_prefabs.begin(); it != m_prefabs.end(); it++)
		{
			obj = *it;
			obj->setPrefab(false);
			DELETE_OBJECT(obj);
			IF_PRINT_DEBUG(SCENE_DEBUG) << "Removed game object: " << obj->getId() << std::endl;
		}

		m_prefabs.clear();
	}
	else
	{
		for (GameObject::List::iterator it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
		{
			obj = *it;
			m_gameObjectsToDestroy.push_back(obj);
			obj->setDestroying(true);
			obj->onDestroy();
			IF_PRINT_DEBUG(SCENE_DEBUG) << "Removed game object: " << obj->getId() << std::endl;
		}
	}
}

bool Scene::hasGameObject(GameObject* obj, bool prefab)
{
	if (prefab)
	{
		for (GameObject::List::iterator it = m_prefabs.begin(); it != m_prefabs.end(); it++)
		{
			if ((*it)->getId() == obj->getId())
				return true;
		}
	}
	else
	{
		for (GameObject::List::iterator it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
		{
			if ((*it)->getId() == obj->getId())
				return true;
		}
	}

	return false;
}

bool Scene::hasGameObject(const std::string& id, bool prefab)
{
	if (prefab)
	{
		for (GameObject::List::iterator it = m_prefabs.begin(); it != m_prefabs.end(); it++)
		{
			if ((*it)->getId() == id)
				return true;
		}
	}
	else
	{
		for (GameObject::List::iterator it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
		{
			if ((*it)->getId() == id)
				return true;
		}
	}

	return false;
}

GameObject* Scene::getGameObject(const std::string& id, bool prefab)
{
	if (prefab)
	{
		for (GameObject::List::iterator it = m_prefabs.begin(); it != m_prefabs.end(); it++)
		{
			if ((*it)->getId() == id)
				return *it;
		}
	}
	else
	{
		for (GameObject::List::iterator it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
		{
			if ((*it)->getId() == id)
				return *it;
		}
	}

	return 0;
}

GameObject* Scene::findGameObject(const std::string& path)
{
	unsigned int from = 0;
    unsigned int p = path.find('/', from);

    while(p - from < 1 && p != std::string::npos)
    {
        from++;
        p = path.find('/', from);
    }

	std::string part = std::string(path, from, p);
	if (part.empty())
		return 0;
	else if (part == ".")
		return findGameObject(std::string(path, p + 1, std::string::npos));
	else if (part == "..")
		return 0;
	{
		GameObject* obj = getGameObject(part);
		if (obj)
			return obj->findChildren(std::string(path, p + 1, std::string::npos));
		else
			return 0;
	}
}

uint32 Scene::getGameObjectCount(bool prefab)
{
	return prefab ? m_prefabs.size() : m_gameObjects.size();
}

GameObject* Scene::instantiatePrefab(const std::string& id)
{
	GameObject* obj = getGameObject(id, true);
	if (!obj)
		return 0;

	GameObject* o = new GameObject();
	o->onDuplicate(obj);
	return o;
}

void Scene::processUpdate(const sf::Time& dt, bool sort)
{
	processAdding();
	processRemoving();

	if (sort)
		m_gameObjects.sort([](GameObject* a, GameObject* b)->bool { return a->getOrder() < b->getOrder(); });

	for (GameObject::List::iterator it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
		(*it)->onUpdate(dt, sf::Transform::Identity, sort);
}

void Scene::processRender(sf::RenderTarget* target)
{
	// It won't work since it requires video manager which is not implemented yet.
	if (!target)
		{}//target = Game::get().getVideoManager()->getWindow();
	if (!target)
	{
		IF_PRINT_WARNING(SCENE_DEBUG) << "cannot process render without target" << std::endl;
		return;
	}

	target->setView(target->getDefaultView());
	sf::RenderTarget*& currentTarget = target;

	for (GameObject::List::iterator it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
		(*it)->onRender(currentTarget);

	target->setView(target->getDefaultView());

	// here we need Camera component which is also missing ;)
	//if (Camera::s_currentRT)
	//	Camera::s_currentRT->display();
}

void Scene::processEvents(const sf::Event& event)
{
	for (GameObject::List::iterator it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
		(*it)->onEvent(event);
}

void Scene::processAdding()
{
	GameObject* o;

	for (GameObject::List::iterator it = m_gameObjectsToCreate.begin(); it != m_gameObjectsToCreate.end(); it++)
	{
		if ((*it))
		{
			o = *it;
			m_gameObjects.push_back(o);
			o->onCreate();
		}
	}

	if (m_gameObjectsToCreate.size() != 0)
	{
		IF_PRINT_DEBUG(SCENE_DEBUG) << "added " << m_gameObjectsToCreate.size() << " objects to scene" << std::endl;
		m_gameObjectsToCreate.clear();
	}
}

void Scene::processRemoving()
{
	GameObject* o;

	for (GameObject::List::iterator it = m_gameObjectsToDestroy.begin(); it != m_gameObjectsToDestroy.end(); it++)
	{
		if ((*it))
		{
			o = *it;
			if (std::find(m_gameObjects.begin(), m_gameObjects.end(), o) == m_gameObjects.end())
				continue;
			m_gameObjects.remove(o);
			o->setPrefab(false);
			DELETE_OBJECT(o);
		}
	}

	if (m_gameObjectsToDestroy.size() != 0)
	{
		IF_PRINT_DEBUG(SCENE_DEBUG) << "removed " << m_gameObjectsToDestroy.size() << " objects from scene" << std::endl;
		m_gameObjectsToDestroy.clear();
	}
}

bool Scene::isWaitingToAdd(GameObject* obj)
{
	for (GameObject::List::iterator it = m_gameObjectsToCreate.begin(); it != m_gameObjectsToCreate.end(); it++)
	{
		if ((*it)->getId() == obj->getId())
			return true;
	}

	return false;
}

bool Scene::isWaitingToRemove(GameObject* obj)
{
	for (GameObject::List::iterator it = m_gameObjectsToDestroy.begin(); it != m_gameObjectsToDestroy.end(); it++)
	{
		if ((*it)->getId() == obj->getId())
			return true;
	}

	return false;
}
