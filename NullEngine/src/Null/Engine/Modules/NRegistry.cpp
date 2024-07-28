
//------------------------------------------------------------------------------
//
// File Name:	NRegistry.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NRegistry.h"
#include "Null/Engine/Submodules/Events/IEvents.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{

	NRegistry::NRegistry() : m_NumEntities(0)
	{
		//AddCreateFunction<PhysicsSystem>([this]() { AddSystem<PhysicsSystem>(); });
		//AddCreateFunction<TransformSystem>([this]() { AddSystem<TransformSystem>(); });
		//AddCreateFunction<SpriteRenderSystem>([this]() { AddSystem<SpriteRenderSystem>(); });
		//AddCreateFunction<AnimationSystem>([this]() { AddSystem<AnimationSystem>(); });
	}

	void NRegistry::Load()
	{
		//std::string filePath = std::string("Data/Systems/") + std::string("Systems") + std::string(".json");
		//// Open the JSON file
		//std::ifstream file(filePath);
		//if (!file.is_open()) {
		//	NLE_ERROR("Error: Could not open file");
		//	return;
		//}


		//JSON j;
		//file >> j;


		//for (auto& sysJson : j["systems"]) {

		//	std::string type = sysJson["type"];

		//	m_Createfunctions.at(type)();
		//}



		//for (auto& system : m_Systems)
		//{
		//	system.second.get()->Load();
		//}
	}

	void NRegistry::Init()
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		SUBSCRIBE_EVENT(EntityDestroyedEvent, &NRegistry::OnEntityDestroyed, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(EntityRemoveComponentEvent, &NRegistry::OnEntityRemoveComponent, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(SceneSwitchEvent, &NRegistry::OnSceneSwitch, eventManager, EventPriority::Medium);
	}

	void NRegistry::Update(float dt)
	{

	}

	void NRegistry::Render()
	{

	}

	void NRegistry::Unload()
	{
	}

	void NRegistry::Shutdown()
	{
	}

	EntityID NRegistry::CreateEntity()
	{
		EntityID id;

		if (!m_RecycledEntity.empty())
		{
			id = m_RecycledEntity.front();
			m_RecycledEntity.pop();
		}
		else
		{
			id = ++m_NumEntities;
		}

		if (!m_EntityToIndexMap.contains(id))
		{
			m_EntityComponentSignatures.push_back(Signature());

			size_t newIndex = m_EntityComponentSignatures.size() - 1;
			m_EntityToIndexMap[id] = newIndex;
			m_IndexToEntityMap[newIndex] = id;
		}

		return id;
	}
	void NRegistry::DestroyEntity(EntityID entityID)
	{
	}
	void NRegistry::AddEntityToSystem(EntityID entityID)
	{
		//const auto& entityComponentSignatures = m_EntityComponentSignatures[m_EntityToIndexMap[entityID]];

		//for (auto& system: m_Systems)
		//{
		//	const auto& systemComponentSignatures = system.second.get()->GetComponentSignature();

		//	bool match = ((entityComponentSignatures & systemComponentSignatures) == systemComponentSignatures);

		//	if(match)
		//		system.second.get()->Add(entityID);
		//}
	}
	const Signature& NRegistry::EntitySignature(EntityID entityID)
	{
		// TODO: insert return statement here
		return m_EntityComponentSignatures[m_EntityToIndexMap[entityID]];
	}

	void NRegistry::OnEntityDestroyed(const EntityDestroyedEvent& e)
	{
		auto& signatures = EntitySignature(e.GetID());

		for (size_t i = 0; i < signatures.size(); i++)
		{
			if (signatures.test(i))
			{
				RemoveComponent(e.GetID(), i);
			}
		}
	}


	void NRegistry::OnEntityRemoveComponent(const EntityRemoveComponentEvent& e)
	{
		RemoveComponent(e.GetID(), e.GetComponentID());
	}
	void NRegistry::OnSceneSwitch(const SceneSwitchEvent& e)
	{
		std::vector<std::unique_ptr<IComponentManager>> m_ComponentManagers;

		for (size_t i = 0; i < m_ComponentManagers.size(); i++)
		{
			m_ComponentManagers[i].get()->Clear();
		}

		m_EntityToIndexMap.clear();
		m_IndexToEntityMap.clear();
		m_EntityComponentSignatures.clear();
	}
}