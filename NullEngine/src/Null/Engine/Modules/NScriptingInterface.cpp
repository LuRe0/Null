
//------------------------------------------------------------------------------
//
// File Name:	NScriptingInterface.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NScriptingInterface.h"
#include "Null/Tools/Trace.h"
#include <nlohmann/json.hpp>
#include "Null/Engine/Submodules/ECS/Entities/Entity.h"
using JSON = nlohmann::json;

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//



namespace NULLENGINE
{
	void NScriptingInterface::Load()
	{
	}
	void NScriptingInterface::Init()
	{

	}
	void NScriptingInterface::RegisterEngineFunctions(sol::state& lua)
	{
		const auto& modules = m_Parent->GetModules();
		Input::RegisterToScripAPI(lua);
        RegisterMathStructures(lua);
      
		for (const auto& mod : modules)
		{
			mod.first->RegisterToScripAPI(lua);
		}

        RegisterEntity(lua);
	}
	void NScriptingInterface::RegisterMathStructures(sol::state& lua)
	{

        // Register glm::vec2
        lua.new_usertype<glm::vec2>("vec2",
            sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
            "x", &glm::vec2::x,
            "y", &glm::vec2::y
        );

        // Register glm::vec3
        lua.new_usertype<glm::vec3>("vec3",
            sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z
        );

        // Register glm::vec4
        lua.new_usertype<glm::vec4>("vec4",
            sol::constructors<glm::vec4(), glm::vec4(float, float, float, float)>(),
            "x", &glm::vec4::x,
            "y", &glm::vec4::y,
            "z", &glm::vec4::z,
            "w", &glm::vec4::w
        );

        // Register glm::mat4
        lua.new_usertype<glm::mat4>("mat4",
            sol::constructors<glm::mat4(), glm::mat4(float)>(),
            "operator*", sol::overload(
                [](const glm::mat4& lhs, const glm::mat4& rhs) { return lhs * rhs; },
                [](const glm::mat4& lhs, const glm::vec4& rhs) { return lhs * rhs; }
            ),
            "operator+", sol::overload(
                [](const glm::mat4& lhs, const glm::mat4& rhs) { return lhs + rhs; }
            )
            // Add more operators or methods if needed
        );
	}
    void NScriptingInterface::RegisterEntity(sol::state& lua)
    {
        lua.new_usertype<Entity>("Entity",
            sol::no_constructor,  // Disable direct construction
            "GetTransform", &Entity::GetComponent,
            "GetTransform", [](Entity& obj) -> TransformComponent* { return &obj.Get<TransformComponent>(); },
            "GetSprite", [](Entity& obj) -> SpriteComponent* { return &obj.Get<SpriteComponent>(); },
            "GetBoxCollider2D", [](Entity& obj) -> BoxCollider2DComponent* { return &obj.Get<BoxCollider2DComponent>(); },
            "GetRigidbody2D", [](Entity& obj) -> Rigidbody2DComponent* { return &obj.Get<Rigidbody2DComponent>(); },
            "GetTag", [](Entity& obj) -> TagComponent* { return &obj.Get<TagComponent>(); },
            "GetAnimation", [](Entity& obj) -> AnimationComponent* { return &obj.Get<AnimationComponent>(); },
            "HasComponent", &Entity::HasComponent,
            "Destroy", &Entity::SetIsDestroyed,
            "ID", &Entity::m_ID,
            "Name", &Entity::m_Name
        );
    }
}