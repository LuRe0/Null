
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



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//


namespace NULLENGINE
{

	void Scene::Load(const JSON& sceneData)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NEntityFactory* entityFactory = NEngine::Instance().Get<NEntityFactory>();
		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		//for (const auto& transitionData : sceneData["transitions"]) {

		//	Transition transition(transitionData["from"], transitionData["to"], transitionData["trigger"]);

		//	m_Transitions.push_back(transition);
		//}

		for (const auto& entityData : sceneData["entities"])
		{
			JsonReader jsonWrapper(entityData);

			Entity entity = entityFactory->CreateEntity(entityData, registry);

			const std::string& name = jsonWrapper.GetString("name", "Entity(" + std::to_string(entity.GetID()) + ")");

			entity.SetName(name);

			if (entityData.contains("components"))
			{

				for (const auto& [componentName, componentData] : entityData["components"].items())
				{
					componentFactory->CreateUniqueComponent(componentName + "Component", componentData, registry, entity.GetID());
				}

			}


			if (entityData.contains("children"))
			{
				HandleChildren(entity, entityData["children"], registry, entityFactory, componentFactory, eventManager);
			}

			m_Entities.push_back(entity);
			eventManager->QueueAsync(std::make_unique<EntityCreatedEvent>(entity.GetID()));

		}

		//eventManager->QueueAsync(std::make_unique<SceneLoadedEvent>(entity.GetID()));

	}


	// Recursive function to handle nested children
	void Scene::HandleChildren(Entity& parentEntity, const nlohmann::json& childrenData, NRegistry* registry,
		NEntityFactory* entityFactory, NComponentFactory* componentFactory, NEventManager* eventManager)
	{
		for (const auto& childData : childrenData)
		{
			Entity childEntity = entityFactory->CreateEntity(childData, registry);

			const std::string& childName = childData.contains("name") ? childData["name"].get<std::string>() : "ChildEntity(" + std::to_string(childEntity.GetID()) + ")";
			childEntity.SetName(childName);


			if (childData.contains("components"))
			{
				for (const auto& [componentName, componentData] : childData["components"].items())
				{
					componentFactory->CreateUniqueComponent(componentName + "Component", componentData, registry, childEntity.GetID());
				}

			}

			registry->AddComponent<ParentComponent>(childEntity.GetID(), parentEntity.GetID());
			auto& childrenComponent = registry->GetOrAddComponent<ChildrenComponent>(parentEntity.GetID());
			childrenComponent.m_Children.push_back(childEntity.GetID());

			if (childData.contains("children"))
			{
				HandleChildren(childEntity, childData["children"], registry, entityFactory, componentFactory, eventManager);
			}

			m_Entities.push_back(childEntity);
			eventManager->QueueAsync(std::make_unique<EntityCreatedEvent>(childEntity.GetID()));

			//eventManager->QueueAsync(std::make_unique<EntityParentedEvent>(parentEntity.m_ID, childEntity.m_ID));

		}
	}

	void Scene::Init()
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();


		SUBSCRIBE_EVENT(EntityLoadedEvent, &Scene::OnEntityAdded, eventManager, EventPriority::High);


		for (const auto& entity : m_Entities)
		{
			registry->AddEntityToSystem(entity.GetID());
		}
	}

	JSON Scene::SerializeChildren(Entity& child, NRegistry* registry, NComponentFactory* componentFactory)
	{


		JSON childEntityJson;
		childEntityJson["name"] = child.GetName();

		if (!child.m_Archetype.empty())
			childEntityJson["archetype"] = child.m_Archetype;

		JSON childComponentsJson;
		auto& childComponents = registry->EntityComponents(child.GetID());

		for (auto& comp : childComponents)
		{

			auto& childComponent = registry->GetComponent(child.GetID(), comp);

			if (!childComponent.m_SerializeToScene)
				continue;

			JSON childCompJson = componentFactory->WriteComponent(&childComponent);

			if (!childCompJson.is_null())
			{
				childComponentsJson.merge_patch(childCompJson);
			}
		}

		childEntityJson["components"] = childComponentsJson;

		if (registry->HasComponent<ChildrenComponent>(child.GetID()))
		{

			const auto& childrenComponent = registry->GetComponent<ChildrenComponent>(child.GetID());
			JSON childrenJson = JSON::array();

			for (auto& childID : childrenComponent.m_Children)
			{
				auto& childEntity = GetEntity(childID);
				auto childEntityJson = SerializeChildren(childEntity, registry, componentFactory);
				childrenJson.push_back(childEntityJson);
			}

			childEntityJson["children"] = childrenJson;
		}

		return childEntityJson;
	}

	EntityID Scene::CreateEmptyEntity(const std::string& name)
	{
		NEntityFactory* entityFactory = NEngine::Instance().Get<NEntityFactory>();
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		Entity entity = entityFactory->CreateEntity(registry);

		entity.SetName(name);

		m_Entities.push_back(entity);

		entity.Add<TransformComponent>();

		eventManager->QueueEvent(std::make_unique<EntityCreatedEvent>(entity.GetID()));

		return entity.GetID();
	}

	EntityID Scene::LoadArchetype(const std::string& name)
	{
		NEntityFactory* entityFactory = NEngine::Instance().Get<NEntityFactory>();
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		Entity& entity = GetEntity(CreateEmptyEntity(name + " (Clone)"));

		entity.SetArchetype(name);

		entityFactory->CloneOrCreateArchetype(name, entity, componentFactory, registry, JSON());

		eventManager->QueueAsync(std::make_unique<EntityCreatedEvent>(entity.GetID()));

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
		auto it = std::find(m_Entities.begin(), m_Entities.end(), entityID);

		return it != m_Entities.end();
	}

	void Scene::Serialize(const std::string& name)
	{
		std::string filePath = "";
		if (name.empty())
			filePath = std::string("../Assets/Scenes/Paths/") + m_Name + std::string(".scene");
		else
			filePath = std::string("../Assets/Scenes/Paths/") + name + std::string(".scene");

		std::ofstream outFile(filePath);

		NLE_CORE_ASSERT(outFile, "Error opening file for writing");

		// Write the JSON object to the file
		//outFile << json.dump(4); // Pretty-print with an indent of 4 spaces

		JSON json;
		JSON entitiesJson = JSON::array();

		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NComponentFactory* compFactory = NEngine::Instance().Get<NComponentFactory>();

		for (auto& entity : m_Entities)
		{
			if (registry->HasComponent<ParentComponent>(entity.GetID()))
				continue;

			JSON entityJson;
			entityJson["name"] = entity.GetName();

			if (!entity.m_Archetype.empty())
				entityJson["archetype"] = entity.m_Archetype;

			JSON componentsJson;
			auto& components = registry->EntityComponents(entity.GetID());

			for (auto& comp : components)
			{

				auto& component = registry->GetComponent(entity.GetID(), comp);

				if (!component.m_SerializeToScene)
					continue;

				JSON compJson = compFactory->WriteComponent(&component);
				if (!compJson.is_null()) 
				{
					componentsJson.merge_patch(compJson); // Merge component JSON into the entity's components JSON
				}
			}

			entityJson["components"] = componentsJson;



			if (registry->HasComponent<ChildrenComponent>(entity.GetID()))
			{
				const auto& childrenComponent = registry->GetComponent<ChildrenComponent>(entity.GetID());
				JSON childrenJson = JSON::array();

				for (auto& childID : childrenComponent.m_Children)
				{
					auto& childEntity = GetEntity(childID);
					auto childEntityJson = SerializeChildren(childEntity, registry, compFactory);
					childrenJson.push_back(childEntityJson);
				}

				entityJson["children"] = childrenJson;
			}


			entitiesJson.push_back(entityJson);
		}

		json["entities"] = entitiesJson;

		outFile << json.dump(4);

		outFile.close();
	}


	void Scene::SerializeArchetype(const std::string& archetype, EntityID entityID)
	{
		std::string filePath = std::string("../Assets/Archetypes/") + archetype + std::string(".ent");

		std::ofstream outFile(filePath);

		NLE_CORE_ASSERT(outFile, "Error opening file for writing");

		// Write the JSON object to the file
		//outFile << json.dump(4); // Pretty-print with an indent of 4 spaces


		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NComponentFactory* compFactory = NEngine::Instance().Get<NComponentFactory>();

		Entity& entity = GetEntity(entityID);

		JSON entityJson;

		entityJson["name"] = entity.GetName();


		JSON componentsJson;
		auto& signature = registry->EntityComponents(entity.GetID());

		for (size_t i = 0; i < signature.size(); i++)
		{

			auto& component = registry->GetComponent(entity.GetID(), signature[i]);
			JSON compJson = compFactory->WriteComponent(&component);
			if (!compJson.is_null())
			{
				componentsJson.merge_patch(compJson); // Merge component JSON into the entity's components JSON
			}
		}

		entityJson["components"] = componentsJson;

		if (registry->HasComponent<ChildrenComponent>(entity.GetID()))
		{
			const auto& childrenComponent = registry->GetComponent<ChildrenComponent>(entity.GetID());
			JSON childrenJson = JSON::array();

			for (auto& childID : childrenComponent.m_Children)
			{
				auto& childEntity = GetEntity(childID);
				auto childEntityJson = SerializeChildren(childEntity, registry, compFactory);
				childrenJson.push_back(childEntityJson);
			}

			entityJson["children"] = childrenJson;
		}

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
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		auto engineState = NEngine::Instance().GetEngineState();

		eventManager->QueueEvent(std::make_unique<EngineEditStateEvent>(NEngine::PAUSE));

		eventManager->QueueEvent(std::make_unique<SceneSwitchEvent>(GetName(), GetName()));

		eventManager->QueueEvent(std::make_unique<EngineEditStateEvent>(engineState));

	}

	void Scene::RegisterToScripAPI(sol::state& lua)
	{
		lua.new_usertype<Scene>(
			"NScene",
			"instantiate_archetype", [](Scene* sc, const std::string& name)
			{
				sc->LoadArchetype(name);
			},
			"reload", [](Scene* sc)
			{
				sc->ReloadScene();
			}
		);
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
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

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