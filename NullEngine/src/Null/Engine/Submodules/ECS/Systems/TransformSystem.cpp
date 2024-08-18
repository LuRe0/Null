
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
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>


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
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = registry->GetComponent<TransformComponent>(entityId);

			if (!transform.m_Enabled)
				continue;

			if (transform.m_Dirty)
			{
				if (transform.m_Scale.z == 0.0)
				{
					transform.m_Scale.z = 1.0f;
				}

				glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform.m_Translation);
				// Calculate rotation matrix (assuming Euler angles in radians)
				glm::mat4 rotationMatrix = glm::toMat4(glm::quat(glm::radians(transform.m_Rotation)));

				glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), transform.m_Scale);

				transform.m_TransformMatrix = translationMatrix * rotationMatrix * scaleMatrix;



				if (registry->HasComponent<ChildrenComponent>(entityId))
				{
					auto& childrenComp = registry->GetComponent<ChildrenComponent>(entityId);
					for (const auto& childId : childrenComp.m_Children)
					{
						UpdateChildTransform(childId, transform, registry);
					}
				}
				else if (registry->HasComponent<ParentComponent>(entityId))
				{
					auto& parentComp = registry->GetComponent<ParentComponent>(entityId);
					// Get the parent's world transform and update the child
					TransformComponent& parentTransform = registry->GetComponent<TransformComponent>(parentComp.m_Parent);
					UpdateChildTransform(entityId, parentTransform, registry);
				}

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

				transform.m_Dirty = false;

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
				"translation", sol::readonly(&TransformComponent::m_Translation),
				"scale", sol::readonly(&TransformComponent::m_Scale),
				"rotation", sol::readonly(&TransformComponent::m_Rotation),
				"set_translation",
				sol::overload(
					[](TransformComponent& transform, float x, float y, float z)
					{
						transform.m_Translation = glm::vec3(x, y, z);
						transform.m_Dirty = transform.m_DirectManipulation = true;
					},
					[](TransformComponent& transform, glm::vec3 newPos)
					{
						transform.m_Translation = newPos;
						transform.m_Dirty = transform.m_DirectManipulation = true;
					}
				),
				"set_scale", sol::overload(
					[](TransformComponent& transform, float x, float y, float z)
					{
						transform.m_Scale = glm::vec3(x, y, z);
						transform.m_Dirty = true;
					},
					[](TransformComponent& transform, glm::vec3 newScale)
					{
						transform.m_Scale = newScale;
						transform.m_Dirty = true;
					}
				),
				"set_rotation", sol::overload(
					[](TransformComponent& transform, float x, float y, float z)
					{
						transform.m_Rotation = glm::vec3(x, y, z);
						transform.m_Dirty = transform.m_DirectManipulation = true;
					},
					[](TransformComponent& transform, glm::vec3 newRot)
					{
						transform.m_Rotation = newRot;
						transform.m_Dirty = transform.m_DirectManipulation = true;
					}
				)
			);
	}


	void TransformSystem::UpdateChildTransform(EntityID childId, const TransformComponent& parentTransform, NRegistry* registry)
	{
		// Retrieve the child's local transform component
		TransformComponent& childTransform = registry->GetComponent<TransformComponent>(childId);

		if (!childTransform.m_Enabled)
			return;

		if (!parentTransform.m_Dirty)
			return;

		//	glm::mat4 inverseParentTransform = glm::inverse(parentTransform.m_TransformMatrix);

		//	childTransform.m_Translation = glm::vec3((inverseParentTransform * glm::vec4(childTransform.m_Translation, 1.0f)));
		//	childTransform.m_Scale = childTransform.m_Scale / parentTransform.m_Scale;

		//	// Extract parent rotation as Euler angles (assume in degrees)
		//	glm::vec3 parentRotationEuler = parentTransform.m_Rotation; // Euler angles in degrees

		//	// Convert parent rotation to quaternion
		//	glm::vec3 parentRotationRadians = glm::radians(parentRotationEuler);
		//	glm::quat parentRotation = glm::quat(glm::yawPitchRoll(parentRotationRadians.y, parentRotationRadians.x, parentRotationRadians.z));

		//	// Convert child rotation from Euler angles (assume in degrees)
		//	glm::vec3 childRotationEuler = childTransform.m_Rotation; // Euler angles in degrees
		//	glm::vec3 childRotationRadians = glm::radians(childRotationEuler);
		//	glm::quat childRotation = glm::quat(glm::yawPitchRoll(childRotationRadians.y, childRotationRadians.x, childRotationRadians.z));

		//	// Compute local rotation by applying the inverse of the parent’s rotation
		//	glm::quat localRotation = glm::normalize(glm::inverse(parentRotation) * childRotation);

		//	// Convert local rotation back to Euler angles
		//	glm::vec3 localRotationEuler = glm::degrees(glm::eulerAngles(localRotation));
		//	childTransform.m_Rotation = localRotationEuler;
		//}

		// Compute the child's local transform matrix
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), childTransform.m_Translation);
		glm::mat4 rotationMatrix = glm::toMat4(glm::quat(glm::radians(childTransform.m_Rotation)));
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), childTransform.m_Scale);

		glm::mat4 localTransformMatrix = translationMatrix * rotationMatrix * scaleMatrix;

		// Combine the parent's world transform with the child's local transform
		childTransform.m_TransformMatrix = parentTransform.m_TransformMatrix * localTransformMatrix;

		// Mark the child's transform as clean
	
		childTransform.m_DirectManipulation = true;

		// Recursively update the child’s children
		if (registry->HasComponent<ChildrenComponent>(childId))
		{
			auto& childrenComp = registry->GetComponent<ChildrenComponent>(childId);
			for (const auto& grandchildId : childrenComp.m_Children)
			{
				UpdateChildTransform(grandchildId, childTransform, registry);
			}
		}
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
			transform.m_Dirty = transform.m_DirectManipulation = true;
		}

		if (ImGui::DragFloat3("Rotation", glm::value_ptr(transform.m_Rotation), 0.5f))
		{
			transform.m_Dirty = transform.m_DirectManipulation = true;
		}

		if (ImGui::DragFloat3("Scale", glm::value_ptr(transform.m_Scale), 0.5f))
		{
			transform.m_Dirty = true;
		}

	}
}