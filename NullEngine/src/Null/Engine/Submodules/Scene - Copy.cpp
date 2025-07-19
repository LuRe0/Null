
//------------------------------------------------------------------------------
//
// File Name:	Scene.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Scene.h"
#include "Null/Tools/JsonWrapper.h"
#include "../../Tools/EntitySerializer.h"
#include "../Modules/NAsyncTaskManager.h"



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//


namespace NULLENGINE
{

	class LoadEntityTask : public IAsyncTask
	{
	public:
		LoadEntityTask(const JSON& entityData, Scene* scene)
			: m_EntityData(entityData), m_Scene(scene), m_IsComplete(false)
		{
		}

		void Process() override
		{
			if (m_IsComplete)
				return;

			NRegistry* registry = NRegistry::Instance();
			NEntityFactory* entityFactory = NEntityFactory::Instance();
			NComponentFactory* componentFactory = NComponentFactory::Instance();
			NEventManager* eventManager = NEventManager::Instance();

			// Deserialize the entity with children, add to scene entities, etc.
			Entity entity = EntitySerializer::DeserializeEntityWithChildren(
				m_EntityData, registry, entityFactory, componentFactory, eventManager, m_Scene);

			m_Scene->AddEntity(entity);

			eventManager->QueueAsync(std::make_unique<EntityCreatedEvent>(entity.GetID()));

			m_IsComplete = true;
		}

		bool IsComplete() const override
		{
			return m_IsComplete;
		}

	private:
		JSON m_EntityData;
		Scene* m_Scene;
		bool m_IsComplete;
	};


	void Scene::Load(const JSON& sceneData)
	{
		NRegistry* registry = NRegistry::Instance();
		NEntityFactory* entityFactory = NEntityFactory::Instance();
		NComponentFactory* componentFactory = NComponentFactory::Instance();
		NEventManager* eventManager =   NEventManager::Instance();

		//for (const auto& transitionData : sceneData["transitions"]) {

		//	Transition transition(transitionData["from"], transitionData["to"], transitionData["trigger"]);

		//	m_Transitions.push_back(transition);
		//}

		NAsyncTaskManager* asyncMgr = NAsyncTaskManager::Instance();
		for (const auto& entityData : sceneData["entities"])
		{
			auto task = std::make_unique<LoadEntityTask>(entityData, this);
			asyncMgr->EnqueueTask(std::move(task));
		}

		//eventManager->QueueAsync(std::make_unique<SceneLoadedEvent>(entity.GetID()));

	}

	void Scene::Init()
	{
		NRegistry* registry = NRegistry::Instance();
		NEventManager* eventManager =   NEventManager::Instance();


		SUBSCRIBE_EVENT(EntityLoadedEvent, &Scene::OnEntityAdded, eventManager, EventPriority::High);


		for (const auto& entity : m_Entities)
		{
			registry->AddEntityToSystem(entity.GetID());
		}
	}


	EntityID Scene::CreateEmptyEntity(const std::string& name)
	{
		NEntityFactory* entityFactory = NEntityFactory::Instance();
		NRegistry* registry = NRegistry::Instance();
		NEventManager* eventManager =   NEventManager::Instance();

		Entity entity = entityFactory->CreateEntity(registry);

		entity.SetName(name);

		m_Entities.push_back(entity);

		entity.Add<TransformComponent>();

		eventManager->QueueEvent(std::make_unique<EntityCreatedEvent>(entity.GetID()));

		return entity.GetID();
	}

	EntityID Scene::LoadArchetype(const std::string& name)
	{
		NEntityFactory* entityFactory = NEntityFactory::Instance();
		NRegistry* registry = NRegistry::Instance();
		NComponentFactory* componentFactory = NComponentFactory::Instance();
		NEventManager* eventManager =   NEventManager::Instance();

		Entity& entity = GetEntity(CreateEmptyEntity(name + " (Clone)"));

		entity.SetArchetype(name);

		entityFactory->CloneOrCreateArchetype(name, entity, componentFactory, registry, JSON());

		//eventManager->QueueAsync(std::make_unique<EntityCreatedEvent>(entity.GetID()));

		return entity.GetID();
	}

	Entity& Scene::GetEntity(const EntityID& entityID)
	{
		auto it = std::find(m_Entities.begin(), m_Entities.end(), entityID);

		NLE_CORE_ASSERT(it != m_Entities.end(), "Entity not found", entityID);

		return *it;
	}

	void Scene::AddEntity(const Entity& entity)
	{
		if (!HasEntity(entity.GetID()))
		{
			m_Entities.push_back(entity);
		}
	}

	bool Scene::HasEntity(const EntityID& entityID)
	{
		if (m_Entities.empty()) return false;

		auto it = std::find(m_Entities.begin(), m_Entities.end(), entityID);

		return it != m_Entities.end();
	}

	void Scene::Serialize(const std::string& name)
	{
		std::string filePath = name.empty()
			? "../Assets/Scenes/Paths/" + m_Name + ".scene"
			: "../Assets/Scenes/Paths/" + name + ".scene";

		std::ofstream outFile(filePath);
		NLE_CORE_ASSERT(outFile, "Error opening file for writing");

		JSON json;
		JSON entitiesJson = JSON::array();

		NRegistry* registry = NRegistry::Instance();
		NComponentFactory* compFactory = NComponentFactory::Instance();

		for (auto& entity : m_Entities)
		{
			if (registry->HasComponent<ParentComponent>(entity.GetID()))
				continue;

			if (registry->HasComponent<TagComponent>(entity.GetID()))
				//if (registry->GetComponent<TagComponent>(entity.GetID()).m_Tags.contains("DoNotSerialize"))
					continue;

			JSON entityJson = EntitySerializer::SerializeEntityWithChildren(entity, registry, compFactory, this);
			entitiesJson.push_back(entityJson);
		}

		json["entities"] = entitiesJson;
		outFile << json.dump(4);
		outFile.close();
	}


	void Scene::SerializeArchetype(const std::string& archetype, EntityID entityID)
	{
		std::string filePath = "../Assets/Archetypes/" + archetype + ".ent";

		std::ofstream outFile(filePath);
		NLE_CORE_ASSERT(outFile, "Error opening file for writing");

		NRegistry* registry = NRegistry::Instance();
		NComponentFactory* compFactory = NComponentFactory::Instance();
		Entity& entity = GetEntity(entityID);

		JSON entityJson = EntitySerializer::SerializeEntityWithChildren(entity, registry, compFactory, this);
		outFile << entityJson.dump(4);
		outFile.close();
	}

	void Scene::SetAsStartScene()
	{
		std::string filePath = std::string("../Assets/Scenes/") + std::string("Init") + std::string(".json");
		JSON initData;

		// Define the JSON content
		initData["startScene"] = m_Name;

		// Open the JSON file
		std::ofstream outputFile(filePath);
		if (!outputFile.is_open()) {
			NLE_CORE_ERROR("Error: Could not create or open file");
			return;
		}

		// Write the JSON content to the file
		outputFile << initData.dump(4); // Dump with indentation of 4 spaces for readability

		// Check if the content was written successfully
		if (outputFile.fail()) {
			NLE_CORE_ERROR("Error: Failed to write JSON content to file");
		}
		else {
			NLE_CORE_INFO("Successfully wrote JSON content to {0}", filePath);
		}

		// Close the file
		outputFile.close();
	}

	void Scene::ReloadScene()
	{
		NEventManager* eventManager =   NEventManager::Instance();

		auto engineState = NEngine::Instance().GetEngineState();

		eventManager->QueueEvent(std::make_unique<EngineEditStateEvent>(NEngine::PAUSE));

		eventManager->QueueEvent(std::make_unique<SceneSwitchEvent>(GetName(), GetName()));

		eventManager->QueueEvent(std::make_unique<EngineEditStateEvent>(engineState));

	}

	void Scene::RegisterToScripAPI(sol::state& lua)
	{
		//lua.new_usertype<Scene>(
		//	"NScene",
		//	"instantiate_archetype", [](Scene* sc, const std::string& name)
		//	{
		//		sc->LoadArchetype(name);
		//	},
		//	"reload", [](Scene* sc)
		//	{
		//		sc->ReloadScene();
		//	}
		//);
	}


	void Scene::RemoveEntity(size_t pos)
	{
		std::swap(m_Entities.back(), m_Entities[pos]);
		m_Entities.pop_back();
	}

	bool Scene::OnEntityAdded(const EntityLoadedEvent& e)
	{
		AddEntity(e.GetEntity());

		return true;
	}

	void Scene::DeleteEntity(EntityID entityID)
	{
		auto it = std::find(m_Entities.begin(), m_Entities.end(), entityID);
		if (it != m_Entities.end())
		{
			it->SetIsDestroyed(true);

			if (it->Has<ChildrenComponent>())
			{
				auto& cComp = it->Get<ChildrenComponent>();
				for (auto& child : cComp.m_Children)
					DeleteEntity(child);
			}
		}
	}


	void Scene::Update(float dt)
	{
		NRegistry* registry = NRegistry::Instance();
		NEventManager* eventManager =   NEventManager::Instance();

		for (size_t i = 0; i < m_Entities.size(); i++)
		{
			if (i < 0)
				return;

			if (m_Entities[i].GetIsDestroyed())
			{
				eventManager->QueueEvent(std::make_unique<EntityDestroyedEvent>(m_Entities[i].GetID()));


				if (m_Entities[i].Has<ParentComponent>())
				{
					auto& pComp = m_Entities[i].Get<ParentComponent>();

					if (!HasEntity(pComp.m_Parent)) continue;

					auto& parentEnt = GetEntity(pComp.m_Parent);

					auto& cComp = parentEnt.Get<ChildrenComponent>();

					auto& children = cComp.m_Children;

					children.erase(
						std::remove(children.begin(), children.end(), m_Entities[i].GetID()),
						children.end()
					);
					bool deez = false;
				}

				RemoveEntity(i);

				--i;
			}
		}

	}

	void Scene::Render()
	{

	}

	void Scene::Unload()
	{

	}

	void Scene::Shutdown()
	{
		m_Entities.clear();
	}

}