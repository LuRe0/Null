
//------------------------------------------------------------------------------
//
// File Name:	Entity.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Entity.h"
#include "Null/Engine/Submodules/ECS/Components/IComponent.h"
#include "Null/Engine/Submodules/Graphics/Texture.h"
#include "Null/Engine/Submodules/Graphics/SpriteSource.h"
#include "Null/Engine/Submodules/Graphics/Mesh.h"





//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{

	Entity::Entity(EntityID id, NRegistry* parent) : m_ID(id), m_Parent(parent)
	{
	}

	 BaseComponent& Entity::GetComponent(const std::string& componentName)
	{
		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();
		auto compID = componentFactory->GetComponentID(componentName);
		return m_Parent->GetComponent(m_ID,compID);
	}

	bool Entity::HasComponent(const std::string& componentName) const
	{
		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();
		auto compID = componentFactory->GetComponentID(componentName);
		return m_Parent->HasComponent(m_ID, compID);
	}

	void Entity::SetIsDestroyed(bool d)
	{
		m_isDestroyed = d;
	}

	bool Entity::GetIsDestroyed() const
	{
		return m_isDestroyed;
	}

	void Entity::SetName(const std::string& name)
	{
		m_Name = name;
	}
	void Entity::SetArchetype(const std::string& name)
	{
		m_Archetype = name;
	}
}