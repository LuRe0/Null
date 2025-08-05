
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
#include "Null/Engine/Submodules/Graphics/Mesh/Mesh.h"
#include "NIncludes.h"




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

	Entity::Entity() : m_ID(0), m_Parent(nullptr)
	{
	}

	bool Entity::HasComponent(const std::string& componentName) const
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();
		auto compID = componentFactory->GetComponentID(componentName);
		return m_Parent->HasComponent(m_ID, compID);
	}
}