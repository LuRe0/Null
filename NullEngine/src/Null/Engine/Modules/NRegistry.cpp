
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
#include "Null/Engine/Submodules/ECS/Systems/PhysicsSystem.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void NRegistry::Load()
	{

	}

	void NRegistry::Init()
	{
		AddSystem<PhysicsSystem>();
	}

	void NRegistry::Update(float dt)
	{
		for (auto& system : m_Systems)
		{
			system.second.get()->Update(dt);
		}
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
		const auto& entityComponentSignatures = m_EntityComponentSignatures[m_EntityToIndexMap[entityID]];

		for (auto& system: m_Systems)
		{
			const auto& systemComponentSignatures = system.second.get()->GetComponentSignature();

			bool match = ((entityComponentSignatures & systemComponentSignatures) == systemComponentSignatures);

			if(match)
				system.second.get()->Add(entityID);
		}
	}
}