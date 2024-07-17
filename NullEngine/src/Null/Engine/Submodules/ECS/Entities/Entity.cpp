
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
		for (const auto& [componentName, componentData] : entityData["components"].items()) {
			if (componentName == "Transform") 
			{
				glm::vec3 translation = { componentData["translation"][0], componentData["translation"][1], componentData["translation"][2] };
				glm::vec3 scale = { componentData["scale"][0], componentData["scale"][1], componentData["scale"][2] };
				glm::vec3 rotation = { componentData["rotation"][0], componentData["rotation"][1], componentData["rotation"][2] };;

				// Assuming you have a TransformComponent
				registry->AddComponent<TransformComponent>(m_ID, translation, scale, rotation);
			}
			else if (componentName == "Sprite") {

				unsigned int frameIndex = componentData["frameindex"];
				std::string texture = componentData["texture"];
				std::string shader = componentData["shader"];
				glm::vec2 dimension = { componentData["dimension"][0], componentData["dimension"][1] };
				glm::vec4 tint = { componentData["tint"][0], componentData["tint"][1], componentData["tint"][2], componentData["tint"][3] };
				// Assuming you have a SpriteComponent
		
				registry->AddComponent<SpriteComponent>(m_ID, frameIndex, new SpriteSource(1,1, new Texture(texture)), new Mesh(0.5f, 0.5f, 1, 1, "deez"), tint, shader);
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