#pragma once
#include "IComponent.h"

class b2Body;
class b2Fixture;

namespace NULLENGINE
{
	struct BoxCollider2DComponent : public BaseComponent
	{
		glm::vec2 m_Offset;
		glm::vec2 m_Scale;
		float m_Density;
		float m_Friction;
		float m_Restitution;
		float m_RestitutionThreshold;
		b2Fixture* m_RuntimeFixture;

		// Constructor with default values
		BoxCollider2DComponent(glm::vec2 offset = { 0.0f, 0.0f },
			glm::vec2 scale = { 50, 50 },
			float density = 1.0f,
			float friction = 0.5f,
			float restitution = 0.0f,
			float restitutionThreshold = 0.5f,
			b2Fixture* runtimeFixture = nullptr)
			: m_Offset(offset),
			m_Scale(scale),
			m_Density(density),
			m_Friction(friction),
			m_Restitution(restitution),
			m_RestitutionThreshold(restitutionThreshold),
			m_RuntimeFixture(runtimeFixture)
		{}

		const std::string Name() { return Component<BoxCollider2DComponent>::TypeName(); };

	};


}