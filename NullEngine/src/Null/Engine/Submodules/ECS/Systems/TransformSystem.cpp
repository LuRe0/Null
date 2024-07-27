
//------------------------------------------------------------------------------
//
// File Name:	TransformSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "TransformSystem.h"
#include "imgui.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

const glm::vec2 GRAVITY(0.0f, -9.81f);

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	TransformSystem::TransformSystem()
	{
		Require<TransformComponent>();

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		componentFactory->Register<TransformComponent>(CreateTransformComponent, [this](Entity& id) { this->ViewTransformComponent(id); });

	}


	void TransformSystem::Load()
	{
	}

	void TransformSystem::Init()
	{
		ISystem::Init();

		//NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();


		//SUBSCRIBE_EVENT(WindowResizeEvent, &TransformSystem::OnWindowResize, eventManager);
	}

	void TransformSystem::Update(float dt)
	{
		NRegistry* m_Parent = NEngine::Instance().Get<NRegistry>();

		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = m_Parent->GetComponent<TransformComponent>(entityId);

			if (transform.m_Dirty) 
			{
				glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform.m_Translation);
				// Calculate rotation matrix (assuming Euler angles in radians)
				glm::mat4 rotationMatrix = glm::mat4(1.0f);

				rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
				rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
				rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

				glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), transform.m_Scale);

				transform.m_TransformMatrix = translationMatrix * rotationMatrix * scaleMatrix;

				transform.m_Dirty = false;
			}
		}
	}


	void TransformSystem::Render() 
	{
	}

	void TransformSystem::Unload()
	{
	}

	void TransformSystem::Shutdown()
	{
	}


	void TransformSystem::CreateTransformComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		auto* comp = static_cast<TransformComponent*>(component);
		JsonWrapper jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			comp->m_Translation = jsonWrapper.GetVec3("translation", { 0.0f, 0.0f, 0.0f });
			comp->m_Scale = jsonWrapper.GetVec3("scale", { 1.0f, 1.0f, 1.0f });
			comp->m_Rotation = jsonWrapper.GetVec3("rotation", { 0.0f, 0.0f, 0.0f });
		}

		componentFactory->AddOrUpdate<TransformComponent>(id, comp, registry, comp->m_Translation, comp->m_Scale, comp->m_Rotation);
	}

	void TransformSystem::ViewTransformComponent(Entity& entity)
	{
		TransformComponent& transform = entity.Get<TransformComponent>();

		ImGui::DragFloat3("Translation", glm::value_ptr(transform.m_Translation), 0.5f);
		ImGui::DragFloat3("Rotation", glm::value_ptr(transform.m_Rotation), 0.5f);
		ImGui::DragFloat3("Scale", glm::value_ptr(transform.m_Scale), 0.5f);

	}
}