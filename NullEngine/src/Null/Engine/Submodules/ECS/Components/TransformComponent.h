#pragma once
#include "IComponent.h"

namespace NULLENGINE
{

	enum TransformFlags : uint8_t
	{
		TransformFlags_Dirty = 1 << 0,
		TransformFlags_DirectManipulation = 1 << 1,
	};

	DEFINE_FLAG_SET(TransformFlagSet,
		uint8_t m_Dirty : 1;
		uint8_t m_DirectManipulation : 1;
		uint8_t m_Reserved : 6;
		);


	struct TransformComponent
	{
		glm::vec3 translation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(50.0f, 50.0f, 1.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled | ComponentFlags_Serialized);
		TransformFlagSet flags = TransformFlagSet(TransformFlags_Dirty);
		glm::mat4 transformMatrix;
	};

}