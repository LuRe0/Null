
//------------------------------------------------------------------------------
//
// File Name:	ISystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "System.h"
#include "Null/Tools/Trace.h"
#include "Null/Engine/Submodules/Events/IEvents.h"



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void ISystem::Load()
	{
	}

	void ISystem::Init()
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NSceneManager* scMan = NEngine::Instance().Get<NSceneManager>();
		Scene* currScene = scMan->GetCurrentScene();

		auto& entities = currScene->GetManagedEntities();

		for (auto& entity : entities)
		{
			CheckEntity(entity.GetID(), registry);
		}

		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();


		SUBSCRIBE_EVENT(EntityCreatedEvent, &ISystem::OnEntityCreate, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(EntityAddComponentEvent, &ISystem::OnEntityCreate, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(EntityRemoveComponentEvent, &ISystem::OnEntityComponentRemoved, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(EntityDestroyedEvent, &ISystem::OnEntityDestroyed, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(SceneSwitchEvent, &ISystem::OnSceneSwitch, eventManager, EventPriority::Medium);
	}



	void ISystem::Update(float dt)
	{
	}



	void ISystem::Unload()
	{
	}

	void ISystem::Shutdown()
	{
	}
	void ISystem::Add(EntityID entity)
	{
		if(std::find(m_Entities.begin(), m_Entities.end(), entity) == m_Entities.end())
			m_Entities.push_back(entity);
	}
	void ISystem::Remove(EntityID entity)
	{
		auto ent = std::find(m_Entities.begin(), m_Entities.end(), entity);
		if (ent != m_Entities.end())
		{
			NLE_CORE_INFO("Entity found in list: {0}", *ent);

			const int index = static_cast<int>(ent - m_Entities.begin());
			std::swap(m_Entities[index], m_Entities.back());

			m_Entities.pop_back();

			NLE_CORE_INFO("Entity Successfully Removed.");
		}
		else
			NLE_CORE_WARN("Entity not found in list!");
	}
	const std::vector<EntityID>& ISystem::GetSystemEntities() const
	{
		// TODO: insert return statement here
		return m_Entities;
	}
	const Signature& ISystem::GetComponentSignature() const
	{
		// TODO: insert return statement here
		return m_ComponentSignatures;
	}

	void ISystem::CheckEntity(EntityID entityID, NRegistry* registry)
	{
		const auto& entityComponentSignatures = registry->EntitySignature(entityID);

		const auto& systemComponentSignatures = GetComponentSignature();

		bool match = ((entityComponentSignatures & systemComponentSignatures) == systemComponentSignatures);

		if (match)
			Add(entityID);
	}

	void ISystem::UpdateEntityList(EntityID entityID, NRegistry* registry)
	{
		const auto& entityComponentSignatures = registry->EntitySignature(entityID);

		const auto& systemComponentSignatures = GetComponentSignature();

		bool match = ((entityComponentSignatures & systemComponentSignatures) == systemComponentSignatures);

		if (!match)
		{
			Remove(entityID);
		}
	}

	void ISystem::OnEntityCreate(const EntityModifiedEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		CheckEntity(e.GetID(), registry);
	}

	void ISystem::OnEntityComponentRemoved(const EntityRemoveComponentEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		UpdateEntityList(e.GetID(), registry);
	}

	void ISystem::OnSceneSwitch(const SceneSwitchEvent& e)
	{
		m_Entities.clear();
	}

	void ISystem::OnEntityDestroyed(const EntityDestroyedEvent& e)
	{
		Remove(e.GetID());
	}


}
