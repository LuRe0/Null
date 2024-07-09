
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
	void Entity::Read(const JSON& entityData)
	{
		for (const auto& [componentName, componentData] : entityData["components"].items()) {
			if (componentName == "Transform") {
				// Assuming you have a TransformComponent
				//entityManager.AddComponent(entity, TransformComponent{ componentData["x"], componentData["y"], componentData["z"] });
			}
			else if (componentName == "Sprite") {
				// Assuming you have a SpriteComponent
				//entityManager.AddComponent(entity, SpriteComponent{ componentData["texture"] });
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