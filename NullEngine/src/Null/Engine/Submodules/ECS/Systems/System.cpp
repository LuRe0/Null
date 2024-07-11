
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
	}

	void ISystem::Update(float dt)
	{
	}

	void ISystem::Render() const
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
			NLE_CORE_ERROR("Entity not found in list!");
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
}
