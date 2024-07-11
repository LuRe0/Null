
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
		for (const auto& [componentName, componentData] : entityData["components"].items()) {
			if (componentName == "Transform") {
				// Assuming you have a TransformComponent
				registry->AddComponent<TransformComponent>(m_ID);
			}
			else if (componentName == "Sprite") {
				// Assuming you have a SpriteComponent
 				registry->AddComponent<SpriteComponent>(m_ID);
			}
			else if (componentName == "Rigidbody") {
				// Assuming you have a SpriteComponent
				registry->AddComponent<Rigidbody2DComponent>(m_ID);
			}
		}
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