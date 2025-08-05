
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
#include "NIncludes.h"


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
		//NRegistry* registry = NRegistry::Instance();
		//NEntityFactory* entityFactory = NEntityFactory::Instance();
		//NComponentFactory* componentFactory = NComponentFactory::Instance();
		//NEventManager* eventManager = NEventManager::Instance();

		////for (const auto& transitionData : sceneData["transitions"]) {

		////	Transition transition(transitionData["from"], transitionData["to"], transitionData["trigger"]);

		////	m_Transitions.push_back(transition);
		////}

		//for (const auto& entityData : sceneData["entities"])
		//{
		//	//NAsyncTaskManager* taskManager = NAsyncTaskManager::Instance();

		//	//taskManager->EnqueueTask(std::make_unique<LoadEntityTask>(entityData, this));

		//	JsonReader jsonWrapper(entityData);

		//	Entity entity = entityFactory->CreateEntity(entityData, registry);

		//	const std::string& archetypeName = jsonWrapper.GetString("archetype", "");

		//	const std::string& name = jsonWrapper.GetString("name", "Entity(" + std::to_string(entity.GetID()) + ")");

		//	entity.SetName(name);

		//	//if (entityData.contains("components"))
		//	//{

		//	//	for (const auto& [componentName, componentData] : entityData["components"].items())
		//	//	{
		//	//		componentFactory->CreateUniqueComponent(componentName + "Component", componentData, registry, entity.GetID());
		//	//	}

		//	//}


		//	if (entityData.contains("children"))
		//	{
		//		HandleChildren(entity, entityData["children"], registry, entityFactory, componentFactory, eventManager);
		//	}

		//	m_Entities.push_back(entity);
		//	eventManager->QueueAsync(std::make_unique<EntityCreatedEvent>(entity.GetID()));
		//}

		//eventManager->QueueAsync(std::make_unique<SceneLoadedEvent>(entity.GetID()));

	}

	void Scene::BuildFromDefinition(const SceneDefinition& definition)
	{
		NRegistry* registry = NRegistry::Instance();
		NEntityFactory* entityFactory = NEntityFactory::Instance();
		NComponentFactory* componentFactory = NComponentFactory::Instance();
		NEventManager* eventManager = NEventManager::Instance();

		for (const auto& [entityName, entityDef] : definition.entities)
		{
			Entity entity(registry->CreateEntity(), registry);

			//entity.Add(STRFROM(entityName));

			if (!entityDef.referencedArchetype)
			{
				//entity.SetArchetype(STRFROM(entityDef.referencedArchetype));

				// Apply archetype components first
				if (NEntityFactory::ArchetypeHelper::HasArchetype(entityDef.referencedArchetype, definition.entities))
				{
					auto& baseComponents = NEntityFactory::ArchetypeHelper::GetArchetypeComponenetList(entityDef.referencedArchetype, definition.entities);
					for (const auto& [typeName, component] : baseComponents)
					{
						const std::string compName = STRFROM(typeName);
						size_t colon = compName.find(':');
						if (colon != std::string::npos)
						{
							std::string baseType = compName.substr(0, colon);
							std::string nameID = compName.substr(colon + 1);
							componentFactory->AddNamedComponentFromBinary(baseType + "Component", STRID(nameID), component, registry, entity.GetID());
						}
						else
						{
							componentFactory->AddComponentFromBinary(compName + "Component", component, registry, entity.GetID());
						}
					}
				}
			}

			// Apply override/diff components
			for (const auto& [typeName, component] : entityDef.components)
			{
				const std::string compName = STRFROM(typeName);
				size_t colon = compName.find(':');
				if (colon != std::string::npos)
				{
					std::string baseType = compName.substr(0, colon);
					std::string nameID = compName.substr(colon + 1);
					componentFactory->AddNamedComponentFromBinary(baseType + "Component", STRID(nameID), component, registry, entity.GetID());
				}
				else
				{
					componentFactory->AddComponentFromBinary(compName+"Component", component, registry, entity.GetID());
				}
			}


			if(!entity.Has<NameComponent>())
			{
				entity.Add<NameComponent>(entityName);
			}

			if (!entity.Has<ArchetypeComponent>())
			{
				entity.Add<ArchetypeComponent>(entityDef.referencedArchetype, 0);
			}

			m_Entities.push_back(entity);
			eventManager->QueueAsync(std::make_unique<EntityCreatedEvent>(entity.GetID()));

			// Recursively apply children
			if (!entityDef.children.empty())
			{
				HandleDefinitionChildren(entity, entityDef.children, registry, entityFactory, componentFactory, eventManager);
			}

			AddEntity(entity);

		}


	}


	void Scene::HandleDefinitionChildren(Entity& parentEntity,
		const std::unordered_map<uint32_t, EntityDefinition>& children,
		NRegistry* registry,
		NEntityFactory* entityFactory,
		NComponentFactory* componentFactory,
		NEventManager* eventManager)
	{
		for (const auto& [childName, childDef] : children)
		{
			Entity childEntity(registry->CreateEntity(), registry);

			//childEntity.SetName(STRFROM(childName));

			if (childDef.referencedArchetype)
			{
				//childEntity.SetArchetype(STRFROM (childDef.referencedArchetype));sdhoijuy;l


				if (entityFactory->HasArchetype(childDef.referencedArchetype))
				{
					auto& baseComponents = NEntityFactory::ArchetypeHelper::GetArchetypeChildComponenetList(childDef.referencedArchetype, childName, children);
					for (const auto& [typeName, component] : baseComponents)
					{
						const std::string compName = STRFROM(typeName);
						size_t colon = compName.find(':');
						if (colon != std::string::npos)
						{
							std::string baseType = compName.substr(0, colon);
							std::string nameID = compName.substr(colon + 1);
							componentFactory->AddNamedComponentFromBinary(baseType + "Component", STRID(nameID), component, registry, childEntity.GetID());
						}
						else
						{
							componentFactory->AddComponentFromBinary(compName+ "Component", component, registry, childEntity.GetID());
						}
					}
				}
			}

			for (const auto& [typeName, component] : childDef.components)
			{
				const std::string compName = STRFROM(typeName);
				size_t colon = compName.find(':');
				if (colon != std::string::npos)
				{
					std::string baseType = compName.substr(0, colon);
					std::string nameID = compName.substr(colon + 1);
					componentFactory->AddNamedComponentFromBinary(baseType + "Component", STRID(nameID), component, registry, childEntity.GetID());
				}
				else
				{
					componentFactory->AddComponentFromBinary(compName+ "Component", component, registry, childEntity.GetID());
				}
			}


			if (childEntity.Has<NameComponent>())
			{
				auto& nameComp = childEntity.Get<NameComponent>();
				nameComp.nameID = (childName);
			}
			else
			{
				childEntity.Add<NameComponent>(childName);
			}

			if (childEntity.Has<ArchetypeComponent>())
			{
				auto& archetypeComp = childEntity.Get<ArchetypeComponent>();
				archetypeComp.archetypeID = childDef.referencedArchetype;
				archetypeComp.parentArchetypeID = 0; // Reset parent archetype ID if needed
			}
			else
			{
				childEntity.Add<ArchetypeComponent>(childDef.referencedArchetype, 0);
			}


			registry->AddComponent<ParentComponent>(childEntity.GetID(), parentEntity.GetID());
			auto& childrenComp = registry->GetOrAddComponent<ChildrenComponent>(parentEntity.GetID());
			childrenComp.m_Children.push_back(childEntity.GetID());

			m_Entities.push_back(childEntity);
			eventManager->QueueAsync(std::make_unique<EntityCreatedEvent>(childEntity.GetID()));

			if (!childDef.children.empty())
			{
				HandleDefinitionChildren(childEntity, childDef.children, registry, entityFactory, componentFactory, eventManager);
			}
		}
	}




	// Recursive function to handle nested children
	//void Scene::HandleChildren(Entity& parentEntity, const nlohmann::json& childrenData, NRegistry* registry,
	//	NEntityFactory* entityFactory, NComponentFactory* componentFactory, NEventManager* eventManager)
	//{
	//	for (const auto& childData : childrenData)
	//	{
	//		Entity childEntity = entityFactory->CreateEntity(childData, registry);

	//		const std::string& childName = childData.contains("name") ? childData["name"].get<std::string>() : "ChildEntity(" + std::to_string(childEntity.GetID()) + ")";
	//		childEntity.SetName(childName);


	//		if (childData.contains("components"))
	//		{
	//			for (const auto& [componentName, componentData] : childData["components"].items())
	//			{
	//				componentFactory->CreateUniqueComponent(componentName + "Component", componentData, registry, childEntity.GetID());
	//			}

	//		}

	//		registry->AddComponent<ParentComponent>(childEntity.GetID(), parentEntity.GetID());
	//		auto& childrenComponent = registry->GetOrAddComponent<ChildrenComponent>(parentEntity.GetID());
	//		childrenComponent.m_Children.push_back(childEntity.GetID());

	//		if (childData.contains("children"))
	//		{
	//			HandleChildren(childEntity, childData["children"], registry, entityFactory, componentFactory, eventManager);
	//		}

	//		m_Entities.push_back(childEntity);
	//		eventManager->QueueAsync(std::make_unique<EntityCreatedEvent>(childEntity.GetID()));

	//		//eventManager->QueueAsync(std::make_unique<EntityParentedEvent>(parentEntity.m_ID, childEntity.m_ID));

	//	}
	//}

	/*void Scene::HandleChildren(Entity& parentEntity, const JSON& childrenData,
		NRegistry* registry, NEntityFactory* entityFactory,
		NComponentFactory* componentFactory, NEventManager* eventManager)
	{
		NAsyncTaskManager* taskManager = NAsyncTaskManager::Instance();

		for (const auto& childData : childrenData)
		{
			taskManager->EnqueueTask(std::make_unique<AddChildEntityTask>(childData, this, parentEntity));
		}
	}*/

	// Recursive function to handle nested children
	void Scene::HandleChildren(Entity& parentEntity, const nlohmann::json& childrenData, NRegistry* registry,
		NEntityFactory* entityFactory, NComponentFactory* componentFactory, NEventManager* eventManager)
	{
		//for (const auto& childData : childrenData)
		//{
		//	Entity childEntity = entityFactory->CreateEntity(childData, registry);

		//	const std::string& childName = childData.contains("name") ? childData["name"].get<std::string>() : "ChildEntity(" + std::to_string(childEntity.GetID()) + ")";
		//	childEntity.SetName(childName);


		//	if (childData.contains("components"))
		//	{
		//		for (const auto& [componentName, componentData] : childData["components"].items())
		//		{
		//			componentFactory->CreateUniqueComponent(componentName + "Component", componentData, registry, childEntity.GetID());
		//		}

		//	}

		//	registry->AddComponent<ParentComponent>(childEntity.GetID(), parentEntity.GetID());
		//	auto& childrenComponent = registry->GetOrAddComponent<ChildrenComponent>(parentEntity.GetID());
		//	childrenComponent.m_Children.push_back(childEntity.GetID());

		//	if (childData.contains("children"))
		//	{
		//		HandleChildren(childEntity, childData["children"], registry, entityFactory, componentFactory, eventManager);
		//	}

		//	m_Entities.push_back(childEntity);
		//	eventManager->QueueAsync(std::make_unique<EntityCreatedEvent>(childEntity.GetID()));

		//	//eventManager->QueueAsync(std::make_unique<EntityParentedEvent>(parentEntity.m_ID, childEntity.m_ID));

		//}
	}


	void Scene::Init()
	{
		NRegistry* registry = NRegistry::Instance();
		NEventManager* eventManager = NEventManager::Instance();


		SUBSCRIBE_EVENT(EntityLoadedEvent, &Scene::OnEntityAdded, eventManager, EventPriority::High);


		for (const auto& entity : m_Entities)
		{
			registry->AddEntityToSystem(entity.GetID());
		}
	}

	JSON Scene::SerializeChildren(Entity& child, NRegistry* registry, NComponentFactory* componentFactory)
	{

		auto& nameComp = child.Get<NameComponent>();
		
		JSON childEntityJson;
		childEntityJson["name"] = STRFROM(nameComp.nameID);

		if (child.Has<ArchetypeComponent>())
		{
			auto& archetypeComp = child.Get<ArchetypeComponent>();
			childEntityJson["archetype"] = STRFROM(archetypeComp.archetypeID);
			childEntityJson["parentArchetype"] = STRFROM(archetypeComp.parentArchetypeID);
		}

		JSON childComponentsJson;
		auto& childComponents = registry->EntityComponents(child.GetID());

		for (auto& comp : childComponents)
		{

			void* childComponent = registry->GetComponent(child.GetID(), static_cast<uint32_t>(comp));


			JSON childCompJson = componentFactory->WriteComponent(comp, childComponent);

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

		return JSON();
	}

	EntityID Scene::CreateEmptyEntity(const std::string& name)
	{
		NEntityFactory* entityFactory = NEntityFactory::Instance();
		NRegistry* registry = NRegistry::Instance();
		NEventManager* eventManager = NEventManager::Instance();

		Entity entity = entityFactory->CreateEntity(registry);

		entity.Add<NameComponent>();
		auto& nameComp = entity.Get<NameComponent>();
		nameComp.nameID = STRID(name);

		m_Entities.push_back(entity);

		entity.Add<TransformComponent>();

		eventManager->QueueEvent(std::make_unique<EntityCreatedEvent>(entity.GetID()));

		return entity.GetID();
	}


	//EntityID Scene::LoadArchetypeDeffered(const std::string& name)
	//{
	//	NEntityFactory* entityFactory = NEntityFactory::Instance();
	//	NRegistry* registry = NRegistry::Instance();
	//	NComponentFactory* componentFactory = NComponentFactory::Instance();
	//	NEventManager* eventManager = NEventManager::Instance();

	//	Entity& entity = GetEntity(CreateEmptyEntity(name + " (Clone)"));

	//	entity.SetArchetype(name);

	//	entityFactory->CloneOrCreateArchetype(name, entity, componentFactory, registry, JSON());

	//	//eventManager->QueueAsync(std::make_unique<EntityCreatedEvent>(entity.GetID()));

	//	return entity.GetID();
	//}


	std::string Scene::GenerateUniqueName(const std::string& baseName)
	{
		std::string cleanedName = baseName;
		auto pos = baseName.find(" (");
		if (pos != std::string::npos)
			cleanedName = baseName.substr(0, pos);

		int& count = m_NameFrequency[cleanedName];

		std::string uniqueName = (count == 0)
			? cleanedName
			: cleanedName + " (" + std::to_string(count) + ")";

		++count;
		return uniqueName;
	}


	EntityID Scene::LoadArchetype(const std::string& name)
	{
		NEntityFactory* entityFactory = NEntityFactory::Instance();
		NRegistry* registry = NRegistry::Instance();
		NComponentFactory* componentFactory = NComponentFactory::Instance();
		NEventManager* eventManager = NEventManager::Instance();

		Entity& entity = GetEntity(CreateEmptyEntity(GenerateUniqueName(name)));

		entity.Add<ArchetypeComponent>();
		auto& archetypeComp = entity.Get<ArchetypeComponent>();
		archetypeComp.archetypeID = STRID(name);

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

		NRegistry* registry = NRegistry::Instance();
		NComponentFactory* compFactory = NComponentFactory::Instance();

		for (auto& entity : m_Entities)
		{
			if (registry->HasComponent<ParentComponent>(entity.GetID()))
				continue;

			if (registry->HasComponent<DoNotSerializeComponent>(entity.GetID()))
					continue;

			JSON entityJson;
			entityJson["name"] = STRFROM(entity.Get<NameComponent>().nameID);

			if (entity.Has<ArchetypeComponent>())
			{
				auto& archetypeComp = entity.Get<ArchetypeComponent>();
				entityJson["archetype"] = STRFROM(archetypeComp.archetypeID);
				entityJson["parentArchetype"] = STRFROM(archetypeComp.parentArchetypeID);
			}

			JSON componentsJson;
			auto& components = registry->EntityComponents(entity.GetID());

			for (auto& comp : components)
			{
				if (!compFactory->IsMultiple(comp))
				{
					auto* component = registry->GetComponent(entity.GetID(), static_cast<uint32_t>(comp));

					JSON compJson = compFactory->WriteComponent(comp, component);

					if (!compJson.is_null())
					{
						componentsJson.merge_patch(compJson); // Merge component JSON into the entity's components JSON
					}
				}
				else
				{
					std::vector<void*> components = registry->GetNamedComponents(entity.GetID(), static_cast<uint32_t>(comp));

					for (auto* component : components)
					{
						JSON compJson = compFactory->WriteComponent(comp, component);
						if (!compJson.is_null())
						{
							componentsJson.merge_patch(compJson); // Merge component JSON into the entity's components JSON
						}
					}
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


		if (NSceneManager::Instance()->HasSceneDefinition(m_Name)) // optional safety check
		{
			NSceneManager::Instance()->CacheSceneDefinition(m_Name, json);
		}



		outFile << json.dump(4);

		outFile.close();
	}


	void Scene::SerializeArchetype(const uint32_t& archetype, EntityID entityID)
	{
		std::string filePath = std::string("../Assets/Archetypes/") + STRFROM(archetype) + std::string(".ent");

		std::ofstream outFile(filePath);

		NLE_CORE_ASSERT(outFile, "Error opening file for writing");

		// Write the JSON object to the file
		//outFile << json.dump(4); // Pretty-print with an indent of 4 spaces


		NRegistry* registry = NRegistry::Instance();
		NComponentFactory* compFactory = NComponentFactory::Instance();

		Entity& entity = GetEntity(entityID);

		JSON entityJson;

		entityJson["name"] = STRFROM(entity.Get<NameComponent>().nameID);



		JSON componentsJson;
		auto& signature = registry->EntityComponents(entity.GetID());

		for (size_t i = 0; i < signature.size(); i++)
		{

			auto* component = registry->GetComponent(entity.GetID(), static_cast<uint32_t>(signature[i]));
			JSON compJson = compFactory->WriteComponent(signature[i], component);
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
		NEventManager* eventManager = NEventManager::Instance();

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
			it->Add<DestroyedComponent>();

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
		NEventManager* eventManager = NEventManager::Instance();

		for (size_t i = 0; i < m_Entities.size(); i++)
		{
			if (i < 0)
				return;

			if (m_Entities[i].Has<DestroyedComponent>())
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
