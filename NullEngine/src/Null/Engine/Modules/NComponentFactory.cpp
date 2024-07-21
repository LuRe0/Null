
//------------------------------------------------------------------------------
//
// File Name:	NNComponentFactory.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NComponentFactory.h"
#include "Null/Engine/Submodules/ECS/Components/IComponent.h"


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    void NComponentFactory::Load()
    {
        AddCreateFunction<TransformComponent>(CreateTransformComponent);
        AddCreateFunction<SpriteComponent>(CreateSpriteComponent);
        AddCreateFunction<Rigidbody2DComponent>(CreateRigidbody2DComponent);
        AddCreateFunction<BoxCollider2DComponent>(CreateBoxCollider2DComponent);
    }


    void NComponentFactory::CreateTransformComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id) {
        auto* comp = static_cast<TransformComponent*>(component);
        JsonWrapper jsonWrapper(json);

        if (!jsonWrapper.Empty())
        {
        comp->m_Translation = jsonWrapper.GetVec3("translation", { 0.0f, 0.0f, 0.0f });
        comp->m_Scale = jsonWrapper.GetVec3("scale", { 1.0f, 1.0f, 1.0f });
        comp->m_Rotation = jsonWrapper.GetVec3("rotation", { 0.0f, 0.0f, 0.0f });}

        registry->AddComponent<TransformComponent>(id, comp->m_Translation, comp->m_Scale, comp->m_Rotation);
    }

    void NComponentFactory::CreateSpriteComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
    {
        NSpriteSourceManager* spritesrcManager = NEngine::Instance().Get<NSpriteSourceManager>();
        NMeshManager* meshManager = NEngine::Instance().Get<NMeshManager>();

        auto* comp = static_cast<SpriteComponent*>(component);
        JsonWrapper jsonWrapper(json);


        if (!jsonWrapper.Empty())
        {
            comp->m_FrameIndex = jsonWrapper.GetInt("frameindex", 0);
            glm::vec2 dimension = jsonWrapper.GetVec2("dimension", { 1.0f, 1.0f });
            comp->m_SpriteSource = spritesrcManager->Create(jsonWrapper.GetString("texture", ""), dimension.x, dimension.y);
            comp->m_ShaderName = jsonWrapper.GetString("shadername", "default");
            comp->m_Mesh = meshManager->Create(jsonWrapper.GetString("meshname", ""));
            comp->m_Color = jsonWrapper.GetVec4("tint", { 1.0f, 1.0f, 1.0f, 1.0f });
        }
        registry->AddComponent<SpriteComponent>(id, comp->m_FrameIndex,comp->m_SpriteSource, comp->m_Mesh, comp->m_Color, comp->m_ShaderName);
    }

    void NComponentFactory::CreateRigidbody2DComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id) {
        auto* comp = static_cast<Rigidbody2DComponent*>(component);
        JsonWrapper jsonWrapper(json);


        if (!jsonWrapper.Empty())
        {
            comp->m_Type = static_cast<Rigidbody2DComponent::BodyType>(jsonWrapper.GetInt("type", 0));
            comp->m_FixedRotation = jsonWrapper.GetBool("fixedRotation", true);
        }
        registry->AddComponent<Rigidbody2DComponent>(id, static_cast<Rigidbody2DComponent::BodyType>(comp->m_Type), comp->m_FixedRotation);
    }

    void NComponentFactory::CreateBoxCollider2DComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id) {
        auto* comp = static_cast<BoxCollider2DComponent*>(component);
        JsonWrapper jsonWrapper(json);


        if (!jsonWrapper.Empty())
        {
            comp->m_Offset = jsonWrapper.GetVec2("offset", { 0.0f, 0.0f });
            comp->m_Scale = jsonWrapper.GetVec2("scale", { 1.0f, 1.0f });
            comp->m_Density = jsonWrapper.GetFloat("density", 1.0f);
            comp->m_Friction = jsonWrapper.GetFloat("friction", 0.5f);
            comp->m_Restitution = jsonWrapper.GetFloat("restitution", 1.0f);
            comp->m_RestitutionThreshold = jsonWrapper.GetFloat("restitutionThreshold", 0.5f);
        }

        registry->AddComponent<BoxCollider2DComponent>(id, comp->m_Offset, comp->m_Scale, comp->m_Density, comp->m_Friction, comp->m_Restitution, comp->m_RestitutionThreshold);
    }
}
