#pragma once
#include "IComponent.h"

class b2Body;
class b2Fixture;

namespace NULLENGINE
{
	struct Collider2DComponent : public BaseComponent
	{
		glm::vec2 m_Offset;
		float m_Density;
		float m_Friction;
		float m_Restitution;
		float m_RestitutionThreshold;
		b2Fixture* m_RuntimeFixture;

		// Constructor with default values
		Collider2DComponent(glm::vec2 offset = { 0.0f, 0.0f },
			float density = 1.0f,
			float friction = 0.5f,
			float restitution = 0.0f,
			float restitutionThreshold = 0.5f,
			b2Fixture* runtimeFixture = nullptr)
			: m_Offset(offset),
			m_Density(density),
			m_Friction(friction),
			m_Restitution(restitution),
			m_RestitutionThreshold(restitutionThreshold),
			m_RuntimeFixture(runtimeFixture)
		{}

		virtual const std::string Name() const = 0;
		virtual const uint32_t ID() const = 0;

	};


	struct BoxCollider2DComponent : public Collider2DComponent
	{
		glm::vec2 m_Scale;

		// Constructor with default values
		BoxCollider2DComponent(glm::vec2 offset = { 0.0f, 0.0f },
			glm::vec2 scale = { 50, 50 },
			float density = 1.0f,
			float friction = 0.5f,
			float restitution = 0.0f,
			float restitutionThreshold = 0.5f,
			b2Fixture* runtimeFixture = nullptr) : Collider2DComponent(offset, density, friction, restitution, restitutionThreshold, runtimeFixture)
			, m_Scale(scale)

		{}

		const std::string Name() const { return Component<BoxCollider2DComponent>::TypeName(); };
		const uint32_t ID() const { return Component<BoxCollider2DComponent>::GetID(); };

	};


	struct CircleCollider2DComponent : public Collider2DComponent
	{
		float m_Radius;

		// Constructor with default values
		CircleCollider2DComponent(glm::vec2 offset = { 0.0f, 0.0f },
			float radius = 50.0f,
			float density = 1.0f,
			float friction = 0.5f,
			float restitution = 0.0f,
			float restitutionThreshold = 0.5f,
			b2Fixture* runtimeFixture = nullptr) : Collider2DComponent(offset, density, friction, restitution, restitutionThreshold, runtimeFixture)
			, m_Radius(radius)

		{}

		const std::string Name() const { return Component<CircleCollider2DComponent>::TypeName(); };
		const uint32_t ID() const { return Component<CircleCollider2DComponent>::GetID(); };

	};



}