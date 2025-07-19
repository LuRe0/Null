
//------------------------------------------------------------------------------
//
// File Name:	NStub.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "EntitySerializer.h"
#include "../Engine/Submodules/Scene.h"



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//



namespace NULLENGINE
{
	//JSON EntitySerializer::SerializeComponents(EntityID id, NRegistry* registry, NComponentFactory* componentFactory)
	//{
	//	JSON componentsJson;
	//	auto& components = registry->EntityComponents(id);

	//	for (auto& comp : components)
	//	{
	//		auto& component = registry->GetComponent(id, static_cast<uint32_t>(comp));
	//		if (!component.m_SerializeToScene)
	//			continue;

	//		JSON compJson = componentFactory->WriteComponent(&component);
	//		if (!compJson.is_null())
	//		{
	//			componentsJson.merge_patch(compJson);
	//		}
	//	}

	//	return componentsJson;
	//}

	//JSON EntitySerializer::SerializeEntity(const Entity& entity, NRegistry* registry, NComponentFactory* componentFactory)
	//{
	//	JSON entityJson;
	//	entityJson["name"] = entity.GetName();

	//	if (!entity.m_Archetype.empty())
	//		entityJson["archetype"] = entity.m_Archetype;

	//	entityJson["components"] = SerializeComponents(entity.GetID(), registry, componentFactory);

	//	return entityJson;
	//}

	//JSON EntitySerializer::SerializeEntityWithChildren(const Entity& entity, NRegistry* registry, NComponentFactory* componentFactory, Scene* scene)
	//{
	//	JSON entityJson = SerializeEntity(entity, registry, componentFactory);

	//	if (registry->HasComponent<ChildrenComponent>(entity.GetID()))
	//	{
	//		const auto& children = registry->GetComponent<ChildrenComponent>(entity.GetID());
	//		JSON childrenJson = JSON::array();

	//		for (auto& childID : children.m_Children)
	//		{
	//			auto& childEntity = scene->GetEntity(childID); // ✅ Use scene
	//			childrenJson.push_back(SerializeEntityWithChildren(childEntity, registry, componentFactory, scene));
	//		}

	//		entityJson["children"] = childrenJson;
	//	}

	//	return entityJson;
	//}


	//Entity EntitySerializer::DeserializeEntity(const JSON& entityJson, NRegistry* registry,
	//	NEntityFactory* entityFactory, NComponentFactory* componentFactory,
	//	NEventManager* eventManager, Scene* scene)
	//{
	//	Entity entity = entityFactory->CreateEntity(entityJson, registry);

	//	const std::string& name = entityJson.contains("name") ? entityJson["name"].get<std::string>() : "Entity(" + std::to_string(entity.GetID()) + ")";
	//	entity.SetName(name);

	//	if (entityJson.contains("archetype"))
	//		entity.SetArchetype(entityJson["archetype"]);

	//	if (entityJson.contains("components"))
	//	{
	//		for (const auto& [compName, compData] : entityJson["components"].items())
	//		{
	//			componentFactory->CreateUniqueComponent(compName + "Component", compData, registry, entity.GetID());
	//		}
	//	}

	//	if (scene)
	//		scene->AddEntity(entity);

	//	if (eventManager)
	//		eventManager->QueueAsync(std::make_unique<EntityCreatedEvent>(entity.GetID()));

	//	return entity;
	//}

	//Entity EntitySerializer::DeserializeEntityWithChildren(const JSON& entityJson, NRegistry* registry,
	//	NEntityFactory* entityFactory, NComponentFactory* componentFactory,
	//	NEventManager* eventManager, Scene* scene)
	//{
	//	Entity entity = DeserializeEntity(entityJson, registry, entityFactory, componentFactory, eventManager, scene);

	//	if (entityJson.contains("children"))
	//	{
	//		const auto& childrenArray = entityJson["children"];
	//		for (const auto& childJson : childrenArray)
	//		{
	//			Entity child = DeserializeEntityWithChildren(childJson, registry, entityFactory, componentFactory, eventManager, scene);

	//			registry->AddComponent<ParentComponent>(child.GetID(), entity.GetID());

	//			auto& cComp = registry->GetOrAddComponent<ChildrenComponent>(entity.GetID());
	//			cComp.m_Children.push_back(child.GetID());
	//		}
	//	}

	//	return entity;
	//}
}
