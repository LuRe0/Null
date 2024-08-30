
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
#include "imgui.h"


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


		SUBSCRIBE_EVENT(EntityCreatedEvent, &ISystem::OnEntityCreate, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(EntityAddComponentEvent, &ISystem::OnEntityCreate, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(EntityRemoveComponentEvent, &ISystem::OnEntityComponentRemoved, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(EntityDestroyedEvent, &ISystem::OnEntityDestroyed, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(SceneSwitchEvent, &ISystem::OnSceneSwitch, eventManager, EventPriority::Medium);
	}



	void ISystem::Update(float dt)
	{
	}



	void ISystem::RenderImGui()
	{
		ImGui::Text("Registered Entities: %d", m_Entities.size());
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
	const SignatureBits& ISystem::GetComponentSignature() const
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

	bool ISystem::OnEntityCreate(const EntityModifiedEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		CheckEntity(e.GetID(), registry);
		return true;
	}

	bool ISystem::OnEntityComponentRemoved(const EntityRemoveComponentEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		UpdateEntityList(e.GetID(), registry);
		return true;
	}

	bool ISystem::OnSceneSwitch(const SceneSwitchEvent& e)
	{
		m_Entities.clear();
		return true;
	}

	bool ISystem::OnEntityDestroyed(const EntityDestroyedEvent& e)
	{
		Remove(e.GetID());
		return true;
	}


	void Signature::set(size_t index, bool value)
	{
		assert(index < m_Bitset.size());

		if (value == m_Bitset.test(index)) return;

		m_Bitset.set(index, value);

		if (m_Bitset.test(index))
		{
			m_Indices.push_back(index);
		}
		else
		{
			m_Indices.erase(std::remove(m_Indices.begin(), m_Indices.end(), index), m_Indices.end());
		}
	}

	void Signature::reset(size_t index)
	{
		assert(index < m_Bitset.size());
		if (m_Bitset.test(index)) 
		{
			m_Bitset.reset(index);
			m_Indices.erase(std::remove(m_Indices.begin(), m_Indices.end(), index), m_Indices.end());
		}
	}

	bool Signature::test(size_t index) const
	{
		return m_Bitset.test(index);
	}

	size_t Signature::size()
	{
		return m_Bitset.size();
	}

	size_t Signature::count()
	{
		return m_Indices.size();
	}

}
