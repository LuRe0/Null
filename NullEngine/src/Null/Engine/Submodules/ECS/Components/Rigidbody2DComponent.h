#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
	enum BodyType
	{
		Static = 0,
		Kinematic,
		Dynamic,
		BodyTypes
	};


	struct Rigidbody2DComponent
	{
		glm::vec2 linearVelocity = glm::vec2(0.0f);
		BodyType type = BodyType::Static;

		float angularVelocity = 0.0f;
		float linearDamping = 0.0f;
		float angularDamping = 0.0f;
		float mass = 0.0f;
		float gravityScale = 1.0f;
		uint32_t runtimeBodyIndex = 0; // [DoNotSerialize]

		bool fixedRotation = false;
		ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled | ComponentFlags_Serialized);
	};




}
