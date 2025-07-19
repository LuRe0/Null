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
		glm::vec3 m_Translation;
		glm::vec3 m_Scale;
		glm::vec3 m_Rotation;
		ComponentFlagSet m_ComponentFlags;
		TransformFlagSet m_Flags;
		glm::mat4 m_TransformMatrix;
	};

}