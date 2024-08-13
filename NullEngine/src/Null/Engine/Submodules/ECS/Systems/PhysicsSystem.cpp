
//------------------------------------------------------------------------------
//
// File Name:	PhysicsSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "PhysicsSystem.h"
#include "imgui.h"
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>
#include <magic_enum/magic_enum.hpp>
#include "Null/Engine/Submodules/Events/IEvents.h"


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//


//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	PhysicsSystem::PhysicsSystem() : m_PhysicsWorld(nullptr)
	{
		Require<TransformComponent>();
		Require<Rigidbody2DComponent>();

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		componentFactory->Register<Rigidbody2DComponent>(CreateRigidbody2DComponent,
			[this](Entity& id) { this->ViewRigidbody2DComponent(id); }, WriteRigidbody2DComponent);
	}
	void PhysicsSystem::Load()
	{
		auto gravity = PixelsToMeters(GRAVITY.x, GRAVITY.y);
		m_PhysicsWorld = new b2World({ gravity.x, gravity.y });
	}

	void PhysicsSystem::Init()
	{
		ISystem::Init();

		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		SUBSCRIBE_EVENT(EntityCreatedEvent, &PhysicsSystem::OnEntityCreated, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(EntityRemoveComponentEvent, &PhysicsSystem::OnEntityComponentRemoved, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(EntityAddComponentEvent, &PhysicsSystem::OnEntityComponentAdded, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(SceneSwitchEvent, &PhysicsSystem::OnSceneSwitched, eventManager, EventPriority::High);

		NRegistry* registry = NEngine::Instance().Get<NRegistry>();


		for (const auto entityId : GetSystemEntities())
		{
			InitializePhysics(entityId, registry);
		}

	}

	void PhysicsSystem::Update(float dt)
	{
		if (m_Simulate)
		{
			RuntimeUpdate(dt);
		}
	}

	void PhysicsSystem::RuntimeUpdate(float dt)
	{
		NRegistry* m_Parent = NEngine::Instance().Get<NRegistry>();

		const int32_t velocityIterations = 6;
		const int32_t positionIterations = 2;

		m_PhysicsWorld->Step(dt, velocityIterations, positionIterations);

		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = m_Parent->GetComponent<TransformComponent>(entityId);
			Rigidbody2DComponent& rb2d = m_Parent->GetComponent<Rigidbody2DComponent>(entityId);

			b2Body* body = rb2d.m_RuntimeBody;


			if (body)
			{

				body->SetAwake(rb2d.m_Enabled);

				if (transform.m_DirectManipulation)
				{
					auto pos = PixelsToMeters(transform.m_Translation.x, transform.m_Translation.y);
					body->SetTransform({ pos.x, pos.y }, transform.m_Rotation.z);

					transform.m_DirectManipulation = false;
				}

				const auto& position = body->GetPosition();

				auto newPos = MetersToPixels(position.x, position.y);

				auto linearVel = body->GetLinearVelocity();

				rb2d.m_LinearVelocity = MetersToPixels(linearVel.x, linearVel.y);

				transform.m_Translation.x = newPos.x;
				transform.m_Translation.y = newPos.y;

				transform.m_Rotation.z = body->GetAngle();

				transform.m_Dirty = true;
			}
		}
	}

	void PhysicsSystem::Render()
	{
	}

	void PhysicsSystem::RenderImGui()
	{
		ISystem::RenderImGui();

		ImGui::Text("Pixels Per Meter:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.0f); // Set the width of the DragFloat
		ImGui::DragFloat("##Pixels Per Meter", &m_Pixels_Per_Meter, 0.5f, 4, 128);
	}

	void PhysicsSystem::Unload()
	{
		delete m_PhysicsWorld;
	}

	void PhysicsSystem::Shutdown()
	{
	}

	void PhysicsSystem::RegisterToScripAPI(sol::state& lua)
	{
		lua.new_usertype<Rigidbody2DComponent>
			(
				"Rigidbody2D",
				sol::no_constructor,
				"type_id", &Component<Rigidbody2DComponent>::GetID,
				"linear_velocity", sol::readonly(&Rigidbody2DComponent::m_LinearVelocity),
				"angular_velocity", sol::readonly(&Rigidbody2DComponent::m_AngularVelocity),
				"gravity_scale", sol::readonly(&Rigidbody2DComponent::m_GravityScale),
				"set_linear_velocity", sol::overload(
					[this](Rigidbody2DComponent& rb2d, float x, float y)
					{
						rb2d.m_LinearVelocity = glm::vec2(x, y);

						auto vel = PixelsToMeters(rb2d.m_LinearVelocity.x, rb2d.m_LinearVelocity.y);

						if (rb2d.m_RuntimeBody)
							rb2d.m_RuntimeBody->SetLinearVelocity({ vel.x, vel.y });
					},
					[this](Rigidbody2DComponent& rb2d, glm::vec2 newVel)
					{
						rb2d.m_LinearVelocity = newVel;

						auto vel = PixelsToMeters(rb2d.m_LinearVelocity.x, rb2d.m_LinearVelocity.y);

						if (rb2d.m_RuntimeBody)
							rb2d.m_RuntimeBody->SetLinearVelocity({ vel.x, vel.y });
					}
				),
				"set_angular_velocity", [](Rigidbody2DComponent& rb2d, float v)
				{
					rb2d.m_AngularVelocity = v;
					if (rb2d.m_RuntimeBody)
						rb2d.m_RuntimeBody->SetAngularVelocity(rb2d.m_AngularVelocity);
				},
				"set_gravity_scale", [](Rigidbody2DComponent& rb2d, float g)
				{
					rb2d.m_GravityScale = g;
					if (rb2d.m_RuntimeBody)
						rb2d.m_RuntimeBody->SetGravityScale(rb2d.m_GravityScale);
				}
		);
	}


	// Converts pixel values to meter values
	const glm::vec2 PhysicsSystem::PixelsToMeters(float xPixels, float yPixels)
	{
		float xMeters = xPixels / m_Pixels_Per_Meter; // Convert pixels to meters
		float yMeters = yPixels / m_Pixels_Per_Meter; // Convert pixels to meters

		return glm::vec2(xMeters, yMeters);
	}

	const float PhysicsSystem::MetersToPixels(float meters)
	{
		return meters * m_Pixels_Per_Meter;
	}

	const float PhysicsSystem::PixelsToMeters(float pixels)
	{
		return pixels / m_Pixels_Per_Meter;
	}

	// Converts meter values to pixel values
	const glm::vec2 PhysicsSystem::MetersToPixels(float xMeters, float yMeters)
	{
		float xPixels = xMeters * m_Pixels_Per_Meter; // Convert meters to pixels
		float yPixels = yMeters * m_Pixels_Per_Meter; // Convert meters to pixels

		return glm::vec2(xPixels, yPixels);
	}




	void PhysicsSystem::CreateRigidbody2DComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		auto* comp = static_cast<Rigidbody2DComponent*>(component);
		JsonReader jsonWrapper(json);


		if (!jsonWrapper.Empty())
		{
			comp->m_Type = static_cast<Rigidbody2DComponent::BodyType>(jsonWrapper.GetInt("type", 0));
			comp->m_FixedRotation = jsonWrapper.GetBool("fixedRotation", true);
			comp->m_LinearVelocity = jsonWrapper.GetVec2("linearVelocity", glm::vec2(0, 0));
			comp->m_AngularVelocity = jsonWrapper.GetFloat("angularVelocity", 0.0f);
			comp->m_LinearDamping = jsonWrapper.GetFloat("linearDamping", 0.0f);
			comp->m_AngularDamping = jsonWrapper.GetFloat("angularDamping", 0.0f);
			comp->m_GravityScale = jsonWrapper.GetFloat("gravityScale", 1.0f);
		}

		componentFactory->AddOrUpdate<Rigidbody2DComponent>(id, comp, registry, static_cast<Rigidbody2DComponent::BodyType>(comp->m_Type), comp->m_FixedRotation,
			nullptr, comp->m_LinearVelocity, comp->m_AngularVelocity, comp->m_LinearDamping, comp->m_AngularDamping,
			comp->m_GravityScale);

	}


	JSON PhysicsSystem::WriteRigidbody2DComponent(BaseComponent* component)
	{
		nlohmann::json json;

		auto& rigidbody = *static_cast<Rigidbody2DComponent*>(component);

		json["Rigidbody2D"]["type"] = rigidbody.m_Type;
		json["Rigidbody2D"]["fixedRotation"] = rigidbody.m_FixedRotation;
		json["Rigidbody2D"]["linearVelocity"] = { rigidbody.m_LinearVelocity.x, rigidbody.m_LinearVelocity.y };
		json["Rigidbody2D"]["angularVelocity"] = rigidbody.m_AngularVelocity;
		json["Rigidbody2D"]["linearDamping"] = rigidbody.m_LinearDamping;
		json["Rigidbody2D"]["angularDamping"] = rigidbody.m_AngularDamping;
		json["Rigidbody2D"]["gravityScale"] = rigidbody.m_GravityScale;

		return json;
	}

	void PhysicsSystem::ViewRigidbody2DComponent(Entity& entity)
	{
		Rigidbody2DComponent& rb2d = entity.Get<Rigidbody2DComponent>();
		if (rb2d.m_RuntimeBody)
		{
			if (ImGui::Checkbox("Fixed Rotation", &rb2d.m_FixedRotation))
				rb2d.m_RuntimeBody->SetFixedRotation(rb2d.m_FixedRotation);

			ImGui::Text(magic_enum::enum_name(rb2d.m_Type).data()); ImGui::SameLine();

			if (ImGui::DragInt("Body Type", reinterpret_cast<int*>(&(rb2d.m_Type)), 1.0f, 0, Rigidbody2DComponent::BodyType::BodyTypes - 1))

				if (rb2d.m_Type > Rigidbody2DComponent::BodyType::BodyTypes - 1)
				{
					rb2d.m_Type = (Rigidbody2DComponent::BodyType)static_cast<int>(Rigidbody2DComponent::BodyType::BodyTypes - 1);
				}

			rb2d.m_RuntimeBody->SetType(static_cast<b2BodyType>(rb2d.m_Type));


			if (ImGui::DragFloat2("Linear Velocity", glm::value_ptr(rb2d.m_LinearVelocity), 0.5f))
			{
				// Handle the change in translation
				auto vel = PixelsToMeters(rb2d.m_LinearVelocity.x, rb2d.m_LinearVelocity.y);

				if (rb2d.m_RuntimeBody)
					rb2d.m_RuntimeBody->SetLinearVelocity({ vel.x, vel.y });

			}

			if (ImGui::DragFloat("Linear Damping", &rb2d.m_LinearDamping, 0.5f))
			{
				if (rb2d.m_RuntimeBody)
					rb2d.m_RuntimeBody->SetLinearDamping(rb2d.m_LinearDamping);
			}

			if (ImGui::DragFloat("Angular Velocity", &rb2d.m_AngularVelocity, 0.5f))
			{
				if (rb2d.m_RuntimeBody)
					rb2d.m_RuntimeBody->SetAngularVelocity(rb2d.m_AngularVelocity);
			}

			if (ImGui::DragFloat("Angular Damping", &rb2d.m_AngularDamping, 0.5f))
			{
				if (rb2d.m_RuntimeBody)
					rb2d.m_RuntimeBody->SetLinearDamping(rb2d.m_AngularDamping);
			}

			if (ImGui::DragFloat("Gravity Scale", &rb2d.m_GravityScale, 0.5f))
			{
				if (rb2d.m_RuntimeBody)
					rb2d.m_RuntimeBody->SetGravityScale(rb2d.m_GravityScale);
			}
		}
		else
		{
			// Display red warning message
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Requires a Collider2D component");
		}

		//update body when data changes
	}


	void PhysicsSystem::OnEntityCreated(const EntityCreatedEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		const auto& entityList = GetSystemEntities();

		if (std::find(entityList.begin(), entityList.end(), e.GetID()) != entityList.end())
			InitializePhysics(e.GetID(), registry);
	}

	void PhysicsSystem::OnEntityComponentRemoved(const EntityRemoveComponentEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		if (e.GetComponentID() == Component<Rigidbody2DComponent>::GetID() || e.GetComponentID() == Component<BoxCollider2DComponent>::GetID())
		{
			if (registry->HasComponent<Rigidbody2DComponent>(e.GetID()))
			{
				Rigidbody2DComponent& rb2d = registry->GetComponent<Rigidbody2DComponent>(e.GetID());

				if (registry->HasComponent<BoxCollider2DComponent>(e.GetID()))
				{
					BoxCollider2DComponent& bc2d = registry->GetComponent<BoxCollider2DComponent>(e.GetID());

					if (bc2d.m_RuntimeFixture)
					{
						rb2d.m_RuntimeBody->DestroyFixture(bc2d.m_RuntimeFixture);

						bc2d.m_RuntimeFixture = nullptr;
					}

				}

				if (rb2d.m_RuntimeBody)
				{
					m_PhysicsWorld->DestroyBody(rb2d.m_RuntimeBody);

					rb2d.m_RuntimeBody = nullptr;
				}

			}
		}
	}

	void PhysicsSystem::OnEntityComponentAdded(const EntityAddComponentEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		if (e.GetComponentID() == Component<Rigidbody2DComponent>::GetID() ||
			e.GetComponentID() == Component<TransformComponent>::GetID() ||
			e.GetComponentID() == Component<BoxCollider2DComponent>::GetID() ||
			e.GetComponentID() == Component<CircleCollider2DComponent>::GetID())
		{
			if (registry->HasComponent<Rigidbody2DComponent>(e.GetID()) &&
				registry->HasComponent<TransformComponent>(e.GetID()) &&
				(registry->HasComponent<BoxCollider2DComponent>(e.GetID()) ||
					registry->HasComponent<CircleCollider2DComponent>(e.GetID())))
			{
				InitializePhysics(e.GetID(), registry);
			}
		}
	}

	void PhysicsSystem::OnSceneSwitched(const SceneSwitchEvent& e)
	{
		for (size_t i = 0; i < m_Entities.size(); i++)
		{
			EntityID id = m_Entities[i];

			NRegistry* registry = NEngine::Instance().Get<NRegistry>();

			Rigidbody2DComponent& rb2d = registry->GetComponent<Rigidbody2DComponent>(id);

			if (registry->HasComponent<BoxCollider2DComponent>(id))
			{
				BoxCollider2DComponent& bc2d = registry->GetComponent<BoxCollider2DComponent>(id);

				if (bc2d.m_RuntimeFixture)
				{
					rb2d.m_RuntimeBody->DestroyFixture(bc2d.m_RuntimeFixture);

					bc2d.m_RuntimeFixture = nullptr;
				}

			}

			if (rb2d.m_RuntimeBody)
			{
				m_PhysicsWorld->DestroyBody(rb2d.m_RuntimeBody);

				rb2d.m_RuntimeBody = nullptr;
			}
		}
	}

	void PhysicsSystem::InitializePhysics(EntityID entityId, NRegistry* registry)
	{
		TransformComponent& transform = registry->GetComponent<TransformComponent>(entityId);
		Rigidbody2DComponent& rb2d = registry->GetComponent<Rigidbody2DComponent>(entityId);

		b2BodyDef bodyDef;

		bodyDef.type = (b2BodyType)rb2d.m_Type;

		auto pos = PixelsToMeters(transform.m_Translation.x, transform.m_Translation.y);
		auto vel = PixelsToMeters(rb2d.m_LinearVelocity.x, rb2d.m_LinearVelocity.y);

		bodyDef.position.Set(pos.x, pos.y);
		bodyDef.linearVelocity.Set(vel.x, vel.y);
		bodyDef.angularVelocity = rb2d.m_AngularVelocity;
		bodyDef.angularDamping = rb2d.m_AngularDamping;
		bodyDef.linearDamping = rb2d.m_LinearDamping;
		bodyDef.gravityScale = rb2d.m_GravityScale;
		bodyDef.angle = transform.m_Rotation.z;


		rb2d.m_RuntimeBody = m_PhysicsWorld->CreateBody(&bodyDef);

		rb2d.m_RuntimeBody->SetFixedRotation(rb2d.m_FixedRotation);

		if (registry->HasComponent<BoxCollider2DComponent>(entityId))
		{
			BoxCollider2DComponent& bc2d = registry->GetComponent<BoxCollider2DComponent>(entityId);

			b2PolygonShape boxShape;

			auto scale = PixelsToMeters(bc2d.m_Scale.x / 2, bc2d.m_Scale.y / 2);
			auto offset = PixelsToMeters(bc2d.m_Offset.x, bc2d.m_Offset.y);

			boxShape.SetAsBox(scale.x, scale.y, b2Vec2(offset.x, offset.y), 0.0f);

			b2FixtureDef fixDef;

			fixDef.shape = &boxShape;
			fixDef.density = bc2d.m_Density;
			fixDef.friction = bc2d.m_Friction;
			fixDef.restitution = bc2d.m_Restitution;
			fixDef.restitutionThreshold = bc2d.m_RestitutionThreshold;

			bc2d.m_RuntimeFixture = rb2d.m_RuntimeBody->CreateFixture(&fixDef);
		}

		if (registry->HasComponent<CircleCollider2DComponent>(entityId))
		{
			CircleCollider2DComponent& cc2d = registry->GetComponent<CircleCollider2DComponent>(entityId);

			b2CircleShape circleShape;

			auto offset = PixelsToMeters(cc2d.m_Offset.x, cc2d.m_Offset.y);
			auto radius = PixelsToMeters(cc2d.m_Radius);


			circleShape.m_p.Set(offset.x, offset.y);
			circleShape.m_radius = radius;

			b2FixtureDef fixDef;

			fixDef.shape = &circleShape;
			fixDef.density = cc2d.m_Density;
			fixDef.friction = cc2d.m_Friction;
			fixDef.restitution = cc2d.m_Restitution;
			fixDef.restitutionThreshold = cc2d.m_RestitutionThreshold;

			cc2d.m_RuntimeFixture = rb2d.m_RuntimeBody->CreateFixture(&fixDef);
		}
	}

}