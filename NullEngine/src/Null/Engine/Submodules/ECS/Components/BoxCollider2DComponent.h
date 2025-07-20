#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
	//struct Collider2DComponent : public BaseComponent
	//{
	//	glm::vec2 m_Offset;
	//	float m_Density;
	//	float m_Friction;
	//	float m_Restitution;
	//	float m_RestitutionThreshold;
	//	b2Fixture* m_RuntimeFixture;

	//	// Constructor with default values
	//	Collider2DComponent(glm::vec2 offset = { 0.0f, 0.0f },
	//		float density = 1.0f,
	//		float friction = 0.5f,
	//		float restitution = 0.0f,
	//		float restitutionThreshold = 0.5f,
	//		b2Fixture* runtimeFixture = nullptr)
	//		: m_Offset(offset),
	//		m_Density(density),
	//		m_Friction(friction),
	//		m_Restitution(restitution),
	//		m_RestitutionThreshold(restitutionThreshold),
	//		m_RuntimeFixture(runtimeFixture)
	//	{}

	//	virtual const std::string Name() const = 0;
	//	virtual const uint32_t ID() const = 0;

	//};


	//struct BoxCollider2DComponent : public Collider2DComponent
	//{
	//	glm::vec2 m_Scale;

	//	// Constructor with default values
	//	BoxCollider2DComponent(glm::vec2 offset = { 0.0f, 0.0f },
	//		glm::vec2 scale = { 50, 50 },
	//		float density = 1.0f,
	//		float friction = 0.5f,
	//		float restitution = 0.0f,
	//		float restitutionThreshold = 0.5f,
	//		b2Fixture* runtimeFixture = nullptr) : Collider2DComponent(offset, density, friction, restitution, restitutionThreshold, runtimeFixture)
	//		, m_Scale(scale)

	//	{}

	//	const std::string Name() const { return Component<BoxCollider2DComponent>::TypeName(); };
	//	const uint32_t ID() const { return Component<BoxCollider2DComponent>::GetID(); };

	//};


	//struct CircleCollider2DComponent : public Collider2DComponent
	//{
	//	float m_Radius;

	//	// Constructor with default values
	//	CircleCollider2DComponent(glm::vec2 offset = { 0.0f, 0.0f },
	//		float radius = 50.0f,
	//		float density = 1.0f,
	//		float friction = 0.5f,
	//		float restitution = 0.0f,
	//		float restitutionThreshold = 0.5f,
	//		b2Fixture* runtimeFixture = nullptr) : Collider2DComponent(offset, density, friction, restitution, restitutionThreshold, runtimeFixture)
	//		, m_Radius(radius)

	//	{}

	//	const std::string Name() const { return Component<CircleCollider2DComponent>::TypeName(); };
	//	const uint32_t ID() const { return Component<CircleCollider2DComponent>::GetID(); };

	//};

	constexpr size_t MAX_COLLIDERS = 8;

	struct CollisionFilter
	{
		uint16_t categoryBits = 0x0001;
		uint16_t maskBits = 0xFFFF;
		int16_t groupIndex = 0;
	};

	struct BoxCollider2D
	{
		glm::vec2 offset = glm::vec2(0.0f);
		glm::vec2 size = glm::vec2(50.0f); // or "scale", depending on your terminology
		float density = 1.0f;
		float friction = 0.5f;
		float restitution = 0.0f;
		float restitutionThreshold = 0.5f;
		uint32_t runtimeFixtureIndex = UINT32_MAX;
		CollisionFilter filter;

	};


	struct CircleCollider2D
	{
		glm::vec2 offset = glm::vec2(0.0f);
		float radius = 50.0f;
		float density = 1.0f;
		float friction = 0.5f;
		float restitution = 0.0f;
		float restitutionThreshold = 0.5f;
		uint32_t runtimeFixtureIndex = UINT32_MAX;
		CollisionFilter filter;

	};


	struct CircleCollider2DComponent
	{
		std::array<CircleCollider2D, MAX_COLLIDERS> colliders{};
		uint8_t colliderCount = 1;
		uint32_t runtimeBodyIndex = UINT32_MAX;
		ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled | ComponentFlags_Serialized);
	};

	struct BoxCollider2DComponent
	{
		std::array<BoxCollider2D, MAX_COLLIDERS> colliders{};
		uint8_t colliderCount = 1;
		uint32_t runtimeBodyIndex = UINT32_MAX;
		ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled | ComponentFlags_Serialized);
	};

}