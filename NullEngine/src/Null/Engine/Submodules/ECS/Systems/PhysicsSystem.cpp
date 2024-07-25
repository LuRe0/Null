
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
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

const glm::vec3 GRAVITY(0.0f, -9.81, 0.0f);
// Conversion factor
const float PIXELS_PER_METER = 64.0f; // 1 meter = 64 pixels

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

		componentFactory->Register<Rigidbody2DComponent>(CreateRigidbody2DComponent);
		componentFactory->Register<BoxCollider2DComponent>(CreateBoxCollider2DComponent);
	}
	void PhysicsSystem::Load()
	{
		auto gravity = PixelsToMeters(GRAVITY.x, GRAVITY.y);
		m_PhysicsWorld = new b2World({gravity.x, gravity.y});
	}

	void PhysicsSystem::Init()
	{
		ISystem::Init();

		NRegistry* m_Parent = NEngine::Instance().Get<NRegistry>();


		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = m_Parent->GetComponent<TransformComponent>(entityId);
			Rigidbody2DComponent& rb2d = m_Parent->GetComponent<Rigidbody2DComponent>(entityId);

			b2BodyDef bodyDef;

			bodyDef.type = (b2BodyType)rb2d.m_Type;

			auto pos = PixelsToMeters(transform.m_Translation.x, transform.m_Translation.y);

			bodyDef.position.Set(pos.x, pos.y);

			bodyDef.angle = transform.m_Rotation.z;

			rb2d.m_RuntimeBody = m_PhysicsWorld->CreateBody(&bodyDef);

			rb2d.m_RuntimeBody->SetFixedRotation(rb2d.m_FixedRotation);

			if (m_Parent->HasComponent<BoxCollider2DComponent>(entityId))
			{
				BoxCollider2DComponent& bc2d = m_Parent->GetComponent<BoxCollider2DComponent>(entityId);

				b2PolygonShape boxShape;

				auto scale = PixelsToMeters(bc2d.m_Scale.x, bc2d.m_Scale.y/2);

				boxShape.SetAsBox(scale.x, scale.y);

				b2FixtureDef fixDef;

				fixDef.shape = &boxShape;
				fixDef.density = bc2d.m_Density;
				fixDef.friction = bc2d.m_Friction;
				fixDef.restitution = bc2d.m_Restitution;
				fixDef.restitutionThreshold = bc2d.m_RestitutionThreshold;

				bc2d.m_RuntimeFixture = rb2d.m_RuntimeBody->CreateFixture(&fixDef);
			}
		}
	}

	void PhysicsSystem::Update(float dt)
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

			const auto& position = body->GetPosition();

			auto newPos = MetersToPixels(position.x, position.y);

			transform.m_Translation.x = newPos.x;
			transform.m_Translation.y = newPos.y;

			transform.m_Rotation.z = body->GetAngle();

			transform.m_Dirty = true;
		}
	}

	void PhysicsSystem::Render()
	{
	}

	void PhysicsSystem::Unload()
	{
		delete m_PhysicsWorld;
	}

	void PhysicsSystem::Shutdown()
	{
	}

	// Converts pixel values to meter values
	const glm::vec2 PhysicsSystem::PixelsToMeters(float xPixels, float yPixels)
	{
		float xMeters = xPixels / PIXELS_PER_METER; // Convert pixels to meters
		float yMeters = yPixels / PIXELS_PER_METER; // Convert pixels to meters

		return glm::vec2(xMeters, yMeters);
	}

	// Converts meter values to pixel values
	const glm::vec2 PhysicsSystem::MetersToPixels(float xMeters, float yMeters)
	{
		float xPixels = xMeters * PIXELS_PER_METER; // Convert meters to pixels
		float yPixels = yMeters * PIXELS_PER_METER; // Convert meters to pixels

		return glm::vec2(xPixels, yPixels);
	}




	void PhysicsSystem::CreateRigidbody2DComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		auto* comp = static_cast<Rigidbody2DComponent*>(component);
		JsonWrapper jsonWrapper(json);


		if (!jsonWrapper.Empty())
		{
			comp->m_Type = static_cast<Rigidbody2DComponent::BodyType>(jsonWrapper.GetInt("type", 0));
			comp->m_FixedRotation = jsonWrapper.GetBool("fixedRotation", true);
		}
		
		componentFactory->AddOrUpdate<Rigidbody2DComponent>(id, comp, registry, static_cast<Rigidbody2DComponent::BodyType>(comp->m_Type), comp->m_FixedRotation);

	}

	void PhysicsSystem::CreateBoxCollider2DComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

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
		componentFactory->AddOrUpdate<BoxCollider2DComponent>(id, comp, registry, comp->m_Offset, comp->m_Scale, comp->m_Density, comp->m_Friction, comp->m_Restitution, comp->m_RestitutionThreshold);
	}
}