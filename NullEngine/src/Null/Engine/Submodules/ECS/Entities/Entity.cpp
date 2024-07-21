
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
	Entity::Entity(EntityID id) : m_ID(id)
	{
	}
	void Entity::Add()
	{
	}
	void Entity::Read(const JSON& entityData, NRegistry* registry)
	{
		NSpriteSourceManager* spritesrcManager = NEngine::Instance().Get<NSpriteSourceManager>();
		NMeshManager* meshManager = NEngine::Instance().Get<NMeshManager>();

	}
	void Entity::Load()
	{
	}
	void Entity::Init()
	{
	}
	void Entity::Shutdown()
	{
	}
	void Entity::SetName(const std::string& name)
	{
		m_Name = name;
	}
}