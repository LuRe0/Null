
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

	void Scene::Load(const std::string& name, const JSON& sceneData)
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
	

			if (entityData.contains("components"))
			{
				const std::string& name = jsonWrapper.GetString("name", "Entity(" + std::to_string(entity.GetID()) + ")");

				entity.SetName(name);

				for (const auto& [componentName, componentData] : entityData["components"].items())
				{
					componentFactory->CreateUniqueComponent(componentName + "Component", componentData, registry, entity.GetID());
				}

			}

			m_Entities.push_back(entity);
			eventManager->TriggerEvent(EntityCreatedEvent(entity.GetID()));

		}

	}




	void Scene::Init()
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		for (const auto& entity : m_Entities)
		{
			registry->AddEntityToSystem(entity.GetID());
		}
	}

	EntityID Scene::CreateEmptyEntity(const std::string& name)
	{
		NEntityFactory* entityFactory = NEngine::Instance().Get<NEntityFactory>();
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		Entity entity = entityFactory->CreateEntity(registry);

		entity.SetName(name);

		m_Entities.push_back(entity);

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

		return entity.GetID();
	}

	Entity& Scene::GetEntity(const EntityID& entityID)
	{
		auto it = std::find(m_Entities.begin(), m_Entities.end(), entityID);

		NLE_CORE_ASSERT(it != m_Entities.end(), "Entity not found", entityID);

		return *it;
	}

	void Scene::Serialize(const std::string& name)
	{
		std::string filePath = "";
		if(name.empty())
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
			JSON entityJson;
			entityJson["name"] = entity.GetName();

			if(!entity.m_Archetype.empty())
				entityJson["archetype"] = entity.m_Archetype;

			JSON componentsJson;
			auto& signature = registry->EntitySignature(entity.GetID());

			for (size_t i = 0; i < signature.size(); i++)
			{
				if (signature.test(i))
				{
					auto& component = registry->GetComponent(entity.GetID(), i);

					if (!component.m_SerializeToScene)
						continue;

					JSON compJson = compFactory->WriteComponent(&component);
					componentsJson.merge_patch(compJson); // Merge component JSON into the entity's components JSON
				}
			}

			entityJson["components"] = componentsJson;
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
				componentsJson.merge_patch(compJson); // Merge component JSON into the entity's components JSON
			
		}

		entityJson["components"] = componentsJson;

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

	void Scene::RegisterToScripAPI(sol::state& lua)
	{
		lua.new_usertype<Scene>(
			"NScene",
			"InstantiateEntity", &Scene::LoadArchetype,
			"GetAllActiveEntities", &Scene::GetManagedEntities
		);
	}


	void Scene::RemoveEntity(size_t pos)
	{
		std::swap(m_Entities.back(), m_Entities[pos]);
		m_Entities.pop_back();
	}

	void Scene::DeleteEntity(EntityID entityID)
	{
		auto it = std::find(m_Entities.begin(), m_Entities.end(), entityID);
		if ( it != m_Entities.end())
		{
			it->SetIsDestroyed(true);
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