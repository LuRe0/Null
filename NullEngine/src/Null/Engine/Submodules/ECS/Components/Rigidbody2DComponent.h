#pragma once
#include "IComponent.h"

class b2Body;
class b2Fixture;

namespace NULLENGINE
{
	struct Rigidbody2DComponent : public BaseComponent
	{
		enum BodyType
		{
			Static = 0,
			Kinematic,
			Dynamic,
			BodyTypes
		};

		BodyType m_Type;
		bool m_FixedRotation;
		b2Body* m_RuntimeBody;

		// New properties
		glm::vec2 m_LinearVelocity;
		float m_AngularVelocity;
		float m_LinearDamping;
		float m_AngularDamping;
		float m_GravityScale;

		// Constructor with default values
		Rigidbody2DComponent(
			BodyType type = Static,
			bool fixedRotation = true,
			b2Body* runtimeBody = nullptr,
			glm::vec2 linVel = glm::vec2(0.0f, 0.0f),
			float angVel = 0.0f,
			float linDamp = 0.0f,
			float angDamp = 0.0f,
			float gravScale = 1.0f
		)
			: m_Type(type),
			m_FixedRotation(fixedRotation),
			m_RuntimeBody(runtimeBody),
			m_LinearVelocity(linVel),
			m_AngularVelocity(angVel),
			m_LinearDamping(linDamp),
			m_AngularDamping(angDamp),
			m_GravityScale(gravScale)
		{}

		const std::string Name() const { return Component<Rigidbody2DComponent>::TypeName(); };
		const uint32_t ID() const { return Component<Rigidbody2DComponent>::GetID(); };

	};
}
