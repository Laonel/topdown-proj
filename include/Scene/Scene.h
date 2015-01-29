#ifndef _SCENE_H_
#define _SCENE_H_

#include "Scene/GameObject.h"

#include "Utils.h"

#include "Filesystem/Archive.h"

class Scene
{
public:

	static void registerComponentFactory(const std::string& id, std::type_index type, Component::OnBuildComponentCallback builder);

	static void unregisterComponentFactory(const std::string& id);
	static void unregisterComponentFactory(std::type_index type);
	static void unregisterComponentFactory(Component::OnBuildComponentCallback builder);

	static void unregisterAllComponentFactories();

	static std::type_index* findComponentFactoryType(const std::string& id);
	static std::type_index* findComponentFactoryType(Component::OnBuildComponentCallback builder);

	static std::string findComponentFactoryId(std::type_index type);
	static std::string findComponentFactoryId(Component::OnBuildComponentCallback builder);

	static Component::OnBuildComponentCallback findComponentFactoryBuilder(const std::string& id);
	static Component::OnBuildComponentCallback findComponentFactoryBuilder(std::type_index type);

	static Component* buildComponent(const std::string& id);
	static Component* buildComponent(std::type_index type);

	static bool init();

	static void shutdown();

	static bool saveScene(const std::string& path);

	static bool loadScene(const std::string& path);

	static void removeScene();

	static bool addGameObject(GameObject* obj, bool prefab = false);

	static void removeGameObject(GameObject* obj, bool prefab = false);
	static void removeGameObject(const std::string& id, bool prefab = false);

	static void removeAllGameObjects(bool prefab = false);

	static bool hasGameObject(GameObject* obj, bool prefab = false);
	static bool hasGameObject(const std::string& id, bool prefab = false);

	static GameObject* getGameObject(const std::string& id, bool prefab = false);

	static GameObject* findGameObject(const std::string& path);

	static uint32 getGameObjectCount(bool prefab = false);

	static GameObject* instantiatePrefab(const std::string& id);

	static void processUpdate(const sf::Time& dt, bool sort = false);
	static void processRender(sf::RenderTarget* target);
	static void processEvents(const sf::Event& event);

	static void processAdding();
	static void processRemoving();

	static bool isWaitingToAdd(GameObject* obj);
	static bool isWaitingToRemove(GameObject* obj);

private:

	struct ComponentFactoryData
	{
		ComponentFactoryData() :
			type(typeid(void))
		{}

		std::type_index type;
		Component::OnBuildComponentCallback builder;
	};

	static std::map<std::string, ComponentFactoryData> m_componentFactory;

	static GameObject::List m_prefabs;
	static GameObject::List m_gameObjects;
	static GameObject::List m_gameObjectsToCreate;
	static GameObject::List m_gameObjectsToDestroy;

	static std::string s_sceneFileExtension;

};

#endif
