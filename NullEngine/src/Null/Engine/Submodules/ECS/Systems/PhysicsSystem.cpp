
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




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

const glm::vec2 GRAVITY(0.0f, -9.81f);

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void PhysicsSystem::Load()
	{
	}

	void PhysicsSystem::Init()
	{
	}

	void PhysicsSystem::Update(float dt)
	{
		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = m_Parent->GetComponent<TransformComponent>(entityId);
			Rigidbody2DComponent& rigidbody = m_Parent->GetComponent<Rigidbody2DComponent>(entityId);

			// Calculate net force (including gravity)
			glm::vec2 netForce = rigidbody.m_Force + (GRAVITY * rigidbody.m_GravityScale * rigidbody.m_Mass);

			// Update acceleration
			rigidbody.m_Acceleration = netForce / rigidbody.m_Mass;

			// Update velocity with drag
			rigidbody.m_Velocity += rigidbody.m_Acceleration * dt;
			rigidbody.m_Velocity *= std::clamp(1.0f - rigidbody.m_Drag * dt, 0.0f, 1.0f);

			// Update position
			rigidbody.m_OldTranslation = transform.Translation;
			transform.Translation += rigidbody.m_Velocity * dt;

			// Reset the force for the next frame
			rigidbody.m_Force = glm::vec2(0.0f);
		}
	}

	void PhysicsSystem::Render() const
	{
	}

	void PhysicsSystem::Unload()
	{
	}

	void PhysicsSystem::Shutdown()
	{
	}
}