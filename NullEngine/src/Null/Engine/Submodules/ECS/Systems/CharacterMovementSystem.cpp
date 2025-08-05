//------------------------------------------------------------------------------
//
// File Name:	CharacterMovementSystem.cpp
// Author(s):	YourName
//
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "CharacterMovementSystem.h"
#include "imgui.h"
#include <nlohmann/json.hpp>
#include "../../../../Tools/ImGuiH.h"
#include <box2d/box2d.h>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>
#include "NIncludes.h"

namespace NULLENGINE
{
	CharacterMovementSystem::CharacterMovementSystem()
	{
		Require < CharacterMovementComponent >();
		//Require < Rigidbody2DComponent > ();

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register < CharacterMovementComponent >(CreateCharacterMovementComponent,
			[this](Entity& entity) { this->ViewCharacterMovementComponent(entity); },
			WriteCharacterMovementComponent, AddCharacterMovementComponent, DiffCharacterMovementComponent);
	}

	void CharacterMovementSystem::Load()
	{
	}

	void CharacterMovementSystem::Init()
	{
		ISystem::Init();

		// Register events here if needed
	}

	void CharacterMovementSystem::Update(float dt)
	{
		NRegistry* registry = NRegistry::Instance();
		PhysicsSystem* physics = PhysicsSystem::Instance();

		// Implement update logic here
		for (EntityID entity : m_Entities)
		{
			auto& move = registry->GetComponent<CharacterMovementComponent>(entity);

			if (!registry->HasComponent<Rigidbody2DComponent>(entity))
				continue;
			// If the entity does not have a Rigidbody2DComponent, skip it

			auto& rb = registry->GetComponent<Rigidbody2DComponent>(entity);

			b2Body* body = physics->GetActiveBody(rb.runtimeBodyIndex);
			if (!body)
				continue;

			b2Vec2 currentVelocity = body->GetLinearVelocity();

			// Target velocity vector based on input and max speed
			b2Vec2 targetVelocity(move.moveInput.x * move.maxMoveSpeed,
				move.moveInput.y * move.maxMoveSpeed);

			// Calculate velocity difference
			b2Vec2 velocityDelta = targetVelocity - currentVelocity;

			// Only apply force if velocity difference is significant
			if (velocityDelta.LengthSquared() > 0.0001f)
			{
				// Normalize delta to get direction of force
				b2Vec2 forceDir = velocityDelta;
				forceDir.Normalize();

				// Calculate force magnitude proportional to acceleration and delta magnitude
				float forceMagnitude = move.acceleration * velocityDelta.Length() * dt;

				// Final force vector
				forceDir *= forceMagnitude;

				// Apply force to center of mass
				body->ApplyForceToCenter(forceDir, true);
			}

			if (std::abs(move.angularInput.x) > 0.0001f)
			{
				float currentAngularVel = body->GetAngularVelocity();
				float targetAngularVel = move.angularInput.x * move.maxAngularSpeed;

				// Check if we're trying to go in the opposite direction OR not at max speed yet
				bool oppositeDirection = (move.angularInput.x > 0 && currentAngularVel < 0) ||
					(move.angularInput.x < 0 && currentAngularVel > 0);
				bool notAtMaxSpeed = std::abs(currentAngularVel) < move.maxAngularSpeed;

				if (oppositeDirection || notAtMaxSpeed) {
					float torque = -targetAngularVel * move.angularAcceleration * dt;
					body->ApplyTorque(torque, true);
				}
			}

			// Handle jumping
			bool canJump = move.flags.m_CanJump != 0;
			bool jumpRequested = move.flags.m_JumpRequested != 0;
			bool isGrounded = move.flags.m_IsGrounded != 0;

			if (canJump && jumpRequested && isGrounded)
			{
				// Set vertical velocity to jump impulse, keep horizontal velocity
				b2Vec2 newVelocity = currentVelocity;
				newVelocity.y = move.jumpImpulse;

				body->ApplyLinearImpulseToCenter(newVelocity, true);

				// Clear jump request and reset grounded flag
				move.flags.m_JumpRequested = 0;
				move.flags.m_IsGrounded = 0;
			}

			// Reset input and jump request every frame (optional: depends on your input system)
			move.moveInput = glm::vec2(0.0f);
			move.angularInput = glm::vec2(0.0f);
		}
	}

	void CharacterMovementSystem::RuntimeUpdate(float dt)
	{
		Update(dt);
	}

	void CharacterMovementSystem::Render()
	{
	}

	void CharacterMovementSystem::Unload()
	{
	}

	void CharacterMovementSystem::Shutdown()
	{
	}

	void CharacterMovementSystem::RegisterToScripAPI(sol::state& lua)
	{
		// Register Lua bindings here
		lua.new_usertype<CharacterMovementComponent>(
			"CharacterMovement",
			sol::no_constructor,

			// Basic fields
			"type_id", &Component<CharacterMovementComponent>::GetID,
			"move_input", &CharacterMovementComponent::moveInput,
			"angular_move_input", &CharacterMovementComponent::angularInput,
			"move_speed", &CharacterMovementComponent::maxMoveSpeed,
			"angular_speed", &CharacterMovementComponent::maxAngularSpeed,
			"acceleration", &CharacterMovementComponent::acceleration,
			"angular_acceleration", &CharacterMovementComponent::angularAcceleration,
			"jump_impulse", &CharacterMovementComponent::jumpImpulse,

			// Flag field accessors
			"can_jump", [](CharacterMovementComponent& c) { return c.flags.IsSet(CharacterMovementFlags_CanJump); },
			"is_grounded", [](CharacterMovementComponent& c) { return c.flags.IsSet(CharacterMovementFlags_IsGrounded); },

			// Input setters
			"set_move_input", sol::overload(
				[](CharacterMovementComponent& c, float x, float y, float scale = 1.0f) {
					glm::vec2 dir = glm::vec2(x, y);
					if (glm::length(dir) > 0.0f) {
						dir = glm::normalize(dir);
					}
					c.moveInput = dir * scale;
				},
				[](CharacterMovementComponent& c, glm::vec2 v, float scale = 1.0f) {
					if (glm::length(v) > 0.0f) {
						v = glm::normalize(v);
					}
					c.moveInput = v;
				},
				[](CharacterMovementComponent& c, float x, float y) {
					glm::vec2 dir = glm::vec2(x, y);
					if (glm::length(dir) > 0.0f) {
						dir = glm::normalize(dir);
					}
					c.moveInput = dir;
				},
				[](CharacterMovementComponent& c, glm::vec2 v) {
					if (glm::length(v) > 0.0f) {
						v = glm::normalize(v);
					}
					c.moveInput = v;
				}
			),
			"set_rotate_input", sol::overload(
				[](CharacterMovementComponent& c, float x, float y, float scale = 1.0f) 
				{
					glm::vec2 dir = glm::vec2(x, y);
					if (glm::length(dir) > 0.0f) {
						dir = glm::normalize(dir);
					}
					c.angularInput = dir * scale;
				},
				[](CharacterMovementComponent& c, glm::vec2 v, float scale = 1.0f) {
					glm::normalize(v);
					c.angularInput = v * scale;
				},
				[](CharacterMovementComponent& c, float x, float y)
				{
					glm::vec2 dir = glm::vec2(x, y);
					if (glm::length(dir) > 0.0f) {
						dir = glm::normalize(dir);
					}
					c.angularInput = dir;
				},
				[](CharacterMovementComponent& c, glm::vec2 v) {
					glm::normalize(v);
					c.angularInput = v;
				}
			),

			// Jump request setter
			"jump", [](CharacterMovementComponent& c) {
				if (c.flags.IsSet(CharacterMovementFlags_CanJump) && c.flags.IsSet(CharacterMovementFlags_IsGrounded))
					c.flags.Set(CharacterMovementFlags_JumpRequested);
			},

			// Manual flag control (optional)
			"set_can_jump", [](CharacterMovementComponent& c, bool enable) {
				c.flags.Set(CharacterMovementFlags_CanJump, enable);
			},
			"set_is_grounded", [](CharacterMovementComponent& c, bool grounded) {
				c.flags.Set(CharacterMovementFlags_IsGrounded, grounded);
			}
		);

	}

	void CharacterMovementSystem::CreateCharacterMovementComponent(void* component, const nlohmann::json& json)
	{
		// Deserialize from json to component

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<CharacterMovementComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{

			// Deserialize component properties
			comp->maxMoveSpeed = jsonWrapper.GetFloat("maxMoveSpeed", comp->maxMoveSpeed);
			comp->acceleration = jsonWrapper.GetFloat("acceleration", comp->acceleration);
			comp->angularAcceleration = jsonWrapper.GetFloat("angularAcceleration", comp->angularAcceleration);
			comp->maxAngularSpeed = jsonWrapper.GetFloat("maxAngularSpeed", comp->maxAngularSpeed);
			comp->jumpImpulse = jsonWrapper.GetFloat("jumpImpulse", comp->jumpImpulse);

			ComponentFlagSet flags;
			flags.Set(ComponentFlags_Enabled);
			flags.Set(ComponentFlags_Serialized);

			comp->componentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);
			comp->flags.m_Flags = jsonWrapper.GetUInt8("flags", comp->flags.m_Flags);
		}


	}

	void CharacterMovementSystem::AddCharacterMovementComponent(void* component, NRegistry* registry, EntityID id)
	{
		// Add or update component in registry
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<CharacterMovementComponent*>(component);



		componentFactory->AddOrUpdate<CharacterMovementComponent>(id, comp, registry, comp->moveInput, comp->angularInput, comp->maxMoveSpeed, comp->acceleration, comp->maxAngularSpeed, comp->angularAcceleration, comp->jumpImpulse, comp->flags, comp->componentFlags);

	}

	JSON CharacterMovementSystem::WriteCharacterMovementComponent(const void* component)
	{
		nlohmann::json json;

		auto& comp = *static_cast<const CharacterMovementComponent*>(component);


		if (!comp.componentFlags.IsSet(ComponentFlags_Serialized))
			return json;

		json["CharacterMovement"]["maxMoveSpeed"] = comp.maxMoveSpeed;
		json["CharacterMovement"]["maxAngularSpeed"] = comp.maxAngularSpeed;
		json["CharacterMovement"]["acceleration"] = comp.acceleration;
		json["CharacterMovement"]["angularAcceleration"] = comp.angularAcceleration;
		json["CharacterMovement"]["jumpImpulse"] = comp.jumpImpulse;
		json["CharacterMovement"]["flags"] = comp.flags.m_Flags;
		json["CharacterMovement"]["ComponentFlags"] = comp.componentFlags.m_Flags;

		return json;
	}

	JSON CharacterMovementSystem::DiffCharacterMovementComponent(const void* base, const void* modified)
	{
		// Calculate diff between base and modified components
		return JSON{};
	}

	void CharacterMovementSystem::ViewCharacterMovementComponent(Entity& entity)
	{
		// ImGui inspector for component
		CharacterMovementComponent& component = entity.Get<CharacterMovementComponent>();
		uint8_t& flags = component.componentFlags.m_Flags;
		// Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
		auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("CharacterMovement", flags, ComponentFlags_Enabled);

		if (remove)
		{
			NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<CharacterMovementComponent>::GetID()));
		}

		if (!open)
			return;

		if (!enabled)
			ImGui::BeginDisabled();

		/// // ImGui controls for the component properties

		bool isEnabled = component.flags.IsSet(CharacterMovementFlags_CanJump);

		if (ImGui::Checkbox("Can Jump", &isEnabled))
		{
			if (isEnabled)
				component.flags.Set(CharacterMovementFlags_CanJump);
			else
				component.flags.Clear(CharacterMovementFlags_CanJump);
		}

		ImGui::InputFloat("Max Move Speed", &component.maxMoveSpeed, 0.1f, 1.0f, "%.2f");
		ImGui::InputFloat("Acceleration", &component.acceleration, 0.1f, 1.0f, "%.2f");
		ImGui::InputFloat("Jump Impulse", &component.jumpImpulse, 0.1f, 1.0f, "%.2f");


		ImGui::InputFloat("Max Angular Move Speed", &component.maxAngularSpeed, 0.1f, 1.0f, "%.2f");
		ImGui::InputFloat("Angular Acceleration", &component.angularAcceleration, 0.1f, 1.0f, "%.2f");


		if (!enabled)
			ImGui::EndDisabled();




		ImGui::TreePop();

		if (!entity.Has<Rigidbody2DComponent>())
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Requires Rigidbody2D component to enable physics");
	}
}
