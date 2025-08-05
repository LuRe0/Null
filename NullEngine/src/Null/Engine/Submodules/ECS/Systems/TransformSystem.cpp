
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

#include "../Entities/Entity.h"
#include "../../../Modules/NSceneManager.h"
#include "../../Scene.h"
#include "../../../../Tools/ImGuiH.h"
#include "NIncludes.h"

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

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register<TransformComponent>(CreateTransformComponent,
			[this](Entity& id) { this->ViewTransformComponent(id); }, WriteTransformComponent,
			AddTransformComponent, DiffTransformComponent);

	}


	void TransformSystem::Load()
	{
	}

	void TransformSystem::Init()
	{
		ISystem::Init();

		NEventManager* eventManager = NEventManager::Instance();


		//SUBSCRIBE_EVENT(WindowResizeEvent, &TransformSystem::OnWindowResize, eventManager);

		SUBSCRIBE_EVENT(EntityParentedEvent, &TransformSystem::OnEntityParented, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(EntitySeparatedEvent, &TransformSystem::OnEntitySeparated, eventManager, EventPriority::High);
	}

	void TransformSystem::UpdateChildTransform(EntityID childId, const TransformComponent& parentTransform, NRegistry* registry)
	{
		// Retrieve the child's local transform component
		TransformComponent& childTransform = registry->GetComponent<TransformComponent>(childId);

		if (!childTransform.componentFlags.IsSet(ComponentFlags_Enabled))
			return;

		// Compute the child's local transform matrix
		//PhysicsSystem::LocalToWorldPos(childTransform, childTransform.m_Translation, childTransform.m_Rotation, childTransform.m_Scale);
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), childTransform.translation);
		glm::mat4 rotationMatrix = glm::toMat4(glm::quat(glm::radians(childTransform.rotation)));
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), childTransform.scale);

		glm::mat4 localTransformMatrix = translationMatrix * rotationMatrix * scaleMatrix;

		// Combine the parent's world transform with the child's local transform
		childTransform.transformMatrix = parentTransform.transformMatrix * localTransformMatrix;

		// Mark the child's transform as clean

		childTransform.flags.Set(TransformFlags_DirectManipulation);

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


	void TransformSystem::Update(float dt)
	{
		NRegistry* registry = NRegistry::Instance();

		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = registry->GetComponent<TransformComponent>(entityId);

			if (!transform.componentFlags.IsSet(ComponentFlags_Enabled))
				continue;

			if (transform.flags.IsSet(TransformFlags_Dirty))
			{
				if (transform.scale.z == 0.0)
				{
					transform.scale.z = 1.0f;
				}

				if (!registry->HasComponent<ParentComponent>(entityId))

				{
					glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform.translation);
					// Calculate rotation matrix (assuming Euler angles in radians)
					glm::mat4 rotationMatrix = glm::toMat4(glm::quat(glm::radians(transform.rotation)));

					glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), transform.scale);

					transform.transformMatrix = translationMatrix * rotationMatrix * scaleMatrix;

				}

				if (registry->HasComponent<ParentComponent>(entityId))
				{
					auto& parentComp = registry->GetComponent<ParentComponent>(entityId);
					// Get the parent's world transform and update the child
					TransformComponent& parentTransform = registry->GetComponent<TransformComponent>(parentComp.m_Parent);
					UpdateChildTransform(entityId, parentTransform, registry);
				}
				if (registry->HasComponent<ChildrenComponent>(entityId))
				{
					auto& childrenComp = registry->GetComponent<ChildrenComponent>(entityId);
					for (const auto& childId : childrenComp.m_Children)
					{
						UpdateChildTransform(childId, transform, registry);
					}
				}

				transform.flags.Clear(TransformFlags_Dirty);

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

		auto get_forward = [](const TransformComponent& transform) -> glm::vec3 {
			float angleRad = glm::radians(transform.rotation.z); // Only Z matters in 2D
			return glm::vec3(std::cos(angleRad), std::sin(angleRad), 0.0f); // 2D forward in XY
			};

		auto get_right = [](const TransformComponent& transform) -> glm::vec3 {
			float angleRad = glm::radians(transform.rotation.z);
			return glm::vec3(std::sin(angleRad), -std::cos(angleRad), 0.0f); // 2D right = rotated 90°
			};

		lua.new_usertype<TransformComponent>
			(
				"Transform",
				sol::no_constructor,
				"type_id", &Component<TransformComponent>::GetID,
				"translation", sol::readonly(&TransformComponent::translation),
				"scale", sol::readonly(&TransformComponent::scale),
				"rotation", sol::readonly(&TransformComponent::rotation),
				"forward", sol::property(get_forward),
				"right", sol::property(get_right),
				"set_translation",
				sol::overload(
					[](TransformComponent& transform, float x, float y, float z)
					{
						transform.translation = glm::vec3(x, y, z);
						transform.flags.Set(TransformFlags_Dirty | TransformFlags_DirectManipulation);
					},
					[](TransformComponent& transform, const glm::vec3& newPos)
					{
						transform.translation = newPos;
						transform.flags.Set(TransformFlags_Dirty | TransformFlags_DirectManipulation);
					}
				),
				"set_scale", sol::overload(
					[](TransformComponent& transform, float x, float y, float z)
					{
						transform.scale = glm::vec3(x, y, z);
						transform.flags.Set(TransformFlags_Dirty);
					},
					[](TransformComponent& transform, glm::vec3 newScale)
					{
						transform.scale = newScale;
						transform.flags.Set(TransformFlags_Dirty);
					}
				),
				"set_rotation", sol::overload(
					[](TransformComponent& transform, float x, float y, float z)
					{
						transform.rotation = glm::vec3(x, y, z);
						transform.flags.Set(TransformFlags_Dirty | TransformFlags_DirectManipulation);
					},
					[](TransformComponent& transform, glm::vec3 newRot)
					{
						transform.rotation = newRot;
						transform.flags.Set(TransformFlags_Dirty | TransformFlags_DirectManipulation);
					}
				)
			);
	}



	void TransformSystem::CreateTransformComponent(void* component, const nlohmann::json& json)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<TransformComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			comp->translation = jsonWrapper.GetVec3("translation", { 0.0f, 0.0f, 0.0f });
			comp->scale = jsonWrapper.GetVec3("scale", { 1.0f, 1.0f, 1.0f });
			comp->rotation = jsonWrapper.GetVec3("rotation", { 0.0f, 0.0f, 0.0f });

			ComponentFlagSet flags;
			flags.Set(ComponentFlags_Enabled);
			flags.Set(ComponentFlags_Serialized);

			TransformFlagSet transformFlags;
			transformFlags.Set(TransformFlags_Dirty);
			comp->flags = transformFlags;
		
			comp->componentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);
		}

	}

	void TransformSystem::AddTransformComponent(void* component, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<TransformComponent*>(component);



		componentFactory->AddOrUpdate<TransformComponent>(id, comp, registry, comp->translation, comp->scale, comp->rotation, comp->componentFlags, comp->flags, comp->transformMatrix);

	}

	JSON TransformSystem::WriteTransformComponent(const void* component)
	{
		nlohmann::json json;

		auto& transform = *static_cast<const TransformComponent*>(component);

		if (!transform.componentFlags.IsSet(ComponentFlags_Serialized))
			return json;

		json["Transform"]["translation"] = { transform.translation.x, transform.translation.y, transform.translation.z };
		json["Transform"]["scale"] = { transform.scale.x, transform.scale.y, transform.scale.z };
		json["Transform"]["rotation"] = { transform.rotation.x, transform.rotation.y, transform.rotation.z };
		json["Transform"]["ComponentFlags"] = transform.componentFlags.m_Flags;

		return json;
	}

	JSON TransformSystem::DiffTransformComponent(const void* base, const void* modified)
	{
		auto* a = static_cast<const TransformComponent*>(base);
		auto* b = static_cast<const TransformComponent*>(modified);

		JSON diff;
		JSON compJson;

		if (a->translation != b->translation)
			compJson["translation"] = { b->translation.x, b->translation.y, b->translation.z };

		if (a->scale != b->scale)
			compJson["scale"] = { b->scale.x, b->scale.y, b->scale.z };

		if (a->rotation != b->rotation)
			compJson["rotation"] = { b->rotation.x, b->rotation.y, b->rotation.z };

		if (a->componentFlags.m_Flags != b->componentFlags.m_Flags)
			diff["ComponentFlags"] = b->componentFlags.m_Flags;

		if (!compJson.empty())
			diff["Transform"] = compJson;

		return diff;
	}


	void TransformSystem::ViewTransformComponent(Entity& entity)
	{
		// Access the transform component
		TransformComponent& transform = entity.Get<TransformComponent>();
		uint8_t& flags = transform.componentFlags.m_Flags;
		// Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
		auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("Transform", flags, ComponentFlags_Enabled);

		if (remove)
		{
			NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<TransformComponent>::GetID()));
		}

		if (!open)
			return;

		if (!enabled)
			ImGui::BeginDisabled();

		// On drag edits, set the dirty & direct manipulation flags
		if (ImGui::DragFloat3("Translation", glm::value_ptr(transform.translation), 0.5f))
		{
			transform.flags.Set(TransformFlags_Dirty);
			transform.flags.Set(TransformFlags_DirectManipulation);
		}

		if (ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotation), 0.5f))
		{
			transform.flags.Set(TransformFlags_Dirty);
			transform.flags.Set(TransformFlags_DirectManipulation);
		}

		if (ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.5f))
		{
			transform.flags.Set(TransformFlags_Dirty);
		}

		if (!enabled)
			ImGui::EndDisabled();


	

		ImGui::TreePop();
	}

	bool TransformSystem::OnEntityParented(const EntityParentedEvent& e)
	{
		auto* sceneManager = NSceneManager::Instance();

		auto& parent = sceneManager->GetCurrentScene()->GetEntity(e.GetParentID());
		auto& child = sceneManager->GetCurrentScene()->GetEntity(e.GetChildID());

		auto& parentTransform = parent.Get<TransformComponent>();
		auto& childTransform = child.Get<TransformComponent>();

		auto rotation = parentTransform.rotation;
		auto scale = parentTransform.scale;
		auto translation = parentTransform.translation;

		PhysicsSystem::LocalToWorldPos(parentTransform, translation, rotation, scale);

		glm::mat4 inverseParentTransform = glm::inverse(parentTransform.transformMatrix);

		childTransform.translation = glm::vec3((inverseParentTransform * glm::vec4(childTransform.translation, 1.0f)));
		childTransform.scale = childTransform.scale / scale;

		// Extract parent rotation as Euler angles (assume in degrees)
		glm::vec3 parentRotationEuler = rotation; // Euler angles in degrees

		// Convert parent rotation to quaternion
		glm::vec3 parentRotationRadians = glm::radians(parentRotationEuler);
		glm::quat parentRotation = glm::quat(glm::yawPitchRoll(parentRotationRadians.y, parentRotationRadians.x, parentRotationRadians.z));

		// Convert child rotation from Euler angles (assume in degrees)
		glm::vec3 childRotationEuler = childTransform.rotation; // Euler angles in degrees
		glm::vec3 childRotationRadians = glm::radians(childRotationEuler);
		glm::quat childRotation = glm::quat(glm::yawPitchRoll(childRotationRadians.y, childRotationRadians.x, childRotationRadians.z));

		// Compute local rotation by applying the inverse of the parent’s rotation
		glm::quat localRotation = glm::normalize(glm::inverse(parentRotation) * childRotation);

		// Convert local rotation back to Euler angles
		glm::vec3 localRotationEuler = glm::degrees(glm::eulerAngles(localRotation));
		childTransform.rotation = localRotationEuler;

		return true;
	}
	bool TransformSystem::OnEntitySeparated(const EntitySeparatedEvent& e)
	{
		auto* sceneManager = NSceneManager::Instance();

		auto& parent = sceneManager->GetCurrentScene()->GetEntity(e.GetParentID());
		auto& child = sceneManager->GetCurrentScene()->GetEntity(e.GetChildID());

		auto& parentTransform = parent.Get<TransformComponent>();
		auto& childTransform = child.Get<TransformComponent>();

		// Apply parent's transformation to child's translation
		childTransform.translation = glm::vec3(parentTransform.transformMatrix * glm::vec4(childTransform.translation, 1.0f));

		// Scale the child's scale by the parent's scale
		childTransform.scale = childTransform.scale * parentTransform.scale;

		// Convert parent rotation to quaternion
		glm::vec3 parentRotationEuler = parentTransform.rotation;
		glm::vec3 parentRotationRadians = glm::radians(parentRotationEuler);
		glm::quat parentRotation = glm::quat(glm::yawPitchRoll(parentRotationRadians.y, parentRotationRadians.x, parentRotationRadians.z));

		// Convert child rotation from Euler angles
		glm::vec3 childRotationEuler = childTransform.rotation;
		glm::vec3 childRotationRadians = glm::radians(childRotationEuler);
		glm::quat childRotation = glm::quat(glm::yawPitchRoll(childRotationRadians.y, childRotationRadians.x, childRotationRadians.z));

		// Compute world rotation by applying the parent's rotation
		glm::quat worldRotation = glm::normalize(parentRotation * childRotation);

		// Convert world rotation back to Euler angles
		glm::vec3 worldRotationEuler = glm::degrees(glm::eulerAngles(worldRotation));
		childTransform.rotation = worldRotationEuler;

		return true;
	}
}