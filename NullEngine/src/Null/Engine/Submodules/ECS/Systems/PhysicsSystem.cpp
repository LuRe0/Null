
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

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	PhysicsSystem::PhysicsSystem() : m_PhysicsWorld(nullptr)
	{
		Require<TransformComponent>();
		Require<Rigidbody2DComponent>();
	}
	void PhysicsSystem::Load()
	{
		auto gravity = PixelsToMeters(GRAVITY.x, GRAVITY.y);
		m_PhysicsWorld = new b2World({gravity.x, gravity.y});
	}

	void PhysicsSystem::Init()
	{
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

	void PhysicsSystem::Render() const
	{
	}

	void PhysicsSystem::Unload()
	{
		delete m_PhysicsWorld;
	}

	void PhysicsSystem::Shutdown()
	{
	}
	const glm::vec2 PhysicsSystem::PixelsToMeters(float xPixels, float yPixels)
	{
		float xMeters = 0.02f * xPixels;
		float yMeters = 0.02f * yPixels;

		return glm::vec2(xMeters, yMeters);
	}
	const glm::vec2 PhysicsSystem::MetersToPixels(float xMeters, float yMeters)
	{
		float xPixels = 50.0f * xMeters;
		float yPixels = 50.0f * yMeters;

		return glm::vec2(xPixels, yPixels);
	}
}