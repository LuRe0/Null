
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
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <box2d/b2_body.h>



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

		componentFactory->Register<TransformComponent>(CreateTransformComponent,
			[this](Entity& id) { this->ViewTransformComponent(id); }, WriteTransformComponent);

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

			if (!transform.m_Enabled)
				continue;

			if (transform.m_Dirty)
			{

				glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform.m_Translation);
				// Calculate rotation matrix (assuming Euler angles in radians)
				glm::mat4 rotationMatrix = glm::toMat4(glm::quat(glm::radians(transform.m_Rotation)));

				glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), transform.m_Scale);

				transform.m_TransformMatrix = translationMatrix * rotationMatrix * scaleMatrix;

				transform.m_Dirty = false;

				//if (m_Parent->HasComponent<Rigidbody2DComponent>(entityId))
				//{
				//	Rigidbody2DComponent& rb2d = m_Parent->GetComponent<Rigidbody2DComponent>(entityId);

				//	if (rb2d.m_RuntimeBody)
				//	{
				//		PhysicsSystem* physicsSys = NEngine::Instance().Get<PhysicsSystem>();
				//		auto pos = physicsSys->PixelsToMeters(transform.m_Translation.x, transform.m_Translation.y);
				//		rb2d.m_RuntimeBody->SetTransform({ pos.x, pos.y }, transform.m_Rotation.z);
				//	}
				//}
			}
		}
	}

	void TransformSystem::RuntimeUpdate(float dt)
	{
		Update(dt);
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

	void TransformSystem::RegisterToScripAPI(sol::state& lua)
	{
		lua.new_usertype<TransformComponent>
			(
				"Transform",
				sol::no_constructor,
				"type_id", &Component<TransformComponent>::GetID,
				"translation", [](TransformComponent& transform)
				{ return std::make_tuple(transform.m_Translation.x, transform.m_Translation.y, transform.m_Translation.z); },
				"scale", [](TransformComponent& transform)
				{ return std::make_tuple(transform.m_Scale.x, transform.m_Scale.y, transform.m_Scale.z); },
				"rotation", [](TransformComponent& transform)
				{ return std::make_tuple(transform.m_Rotation.x, transform.m_Rotation.y, transform.m_Rotation.z); },
				"set_translation", [](TransformComponent& transform, float x, float y, float z)
				{
					transform.m_Translation = glm::vec3(x, y, z);
					transform.m_Dirty = transform.m_DirectManipulation = true;
				},
				"set_scale", [](TransformComponent& transform, float x, float y, float z)
				{
					transform.m_Scale = glm::vec3(x, y, z);
					transform.m_Dirty = true;
				},
				"set_rotation", [](TransformComponent& transform, float x, float y, float z)
				{
					transform.m_Scale = glm::vec3(x, y, z);
					transform.m_Dirty = transform.m_DirectManipulation = true;
				}
		);
	}


	void TransformSystem::CreateTransformComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		auto* comp = static_cast<TransformComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			comp->m_Translation = jsonWrapper.GetVec3("translation", { 0.0f, 0.0f, 0.0f });
			comp->m_Scale = jsonWrapper.GetVec3("scale", { 1.0f, 1.0f, 1.0f });
			comp->m_Rotation = jsonWrapper.GetVec3("rotation", { 0.0f, 0.0f, 0.0f });
		}

		componentFactory->AddOrUpdate<TransformComponent>(id, comp, registry, comp->m_Translation, comp->m_Scale, comp->m_Rotation);
	}

	JSON TransformSystem::WriteTransformComponent(BaseComponent* component)
	{
		nlohmann::json json;

		auto& transform = *static_cast<TransformComponent*>(component);

		json["Transform"]["translation"] = { transform.m_Translation.x, transform.m_Translation.y, transform.m_Translation.z };
		json["Transform"]["scale"] = { transform.m_Scale.x, transform.m_Scale.y, transform.m_Scale.z };
		json["Transform"]["rotation"] = { transform.m_Rotation.x, transform.m_Rotation.y, transform.m_Rotation.z };

		return json;
	}

	void TransformSystem::ViewTransformComponent(Entity& entity)
	{
		TransformComponent& transform = entity.Get<TransformComponent>();


		if (ImGui::DragFloat3("Translation", glm::value_ptr(transform.m_Translation), 0.5f))
		{
			// Handle the change in translation
			//if (entity.Has<Rigidbody2DComponent>())
			//{
			//	Rigidbody2DComponent& rb2d = entity.Get<Rigidbody2DComponent>();
			//	
			//	auto pos = physicsSys->PixelsToMeters(transform.m_Translation.x, transform.m_Translation.y);
			//	if(rb2d.m_RuntimeBody)
			//		rb2d.m_RuntimeBody->SetTransform({ pos.x, pos.y }, transform.m_Rotation.z);
			//}

			transform.m_Dirty = transform.m_DirectManipulation = true;

		}

		if (ImGui::DragFloat3("Rotation", glm::value_ptr(transform.m_Rotation), 0.5f))
		{
			// Handle the change in rotation
			//if (entity.Has<Rigidbody2DComponent>())
			//{
			//	Rigidbody2DComponent& rb2d = entity.Get<Rigidbody2DComponent>();

			//	auto pos = physicsSys->PixelsToMeters(transform.m_Translation.x, transform.m_Translation.y);
			//	if(rb2d.m_RuntimeBody)
			//		rb2d.m_RuntimeBody->SetTransform(rb2d.m_RuntimeBody->GetPosition(), transform.m_Rotation.z);
			//}

			transform.m_Dirty = transform.m_DirectManipulation = true;

		}

		if (ImGui::DragFloat3("Scale", glm::value_ptr(transform.m_Scale), 0.5f))
		{
			// Handle the change in scale
			transform.m_Dirty = true;
		}

	}
}