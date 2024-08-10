#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
	struct  TransformComponent : public BaseComponent
	{
		glm::vec3 m_Translation;
		glm::vec3 m_Scale;
		glm::vec3 m_Rotation;
		bool m_Dirty;
		bool m_DirectManipulation = false;
		glm::mat4 m_TransformMatrix;


		// Constructor with default values
		TransformComponent(glm::vec3 translation = glm::vec3(0.0f),
			glm::vec3 scale = glm::vec3(50.0f),
			glm::vec3 rotation = glm::vec3(0.0f),
			bool dirty = true)
			: m_Translation(translation),
			m_Scale(scale),
			m_Rotation(rotation),
			m_Dirty(dirty),
			m_TransformMatrix(glm::mat4(1.0f)) // Identity matrix by default
		{}

		const std::string Name() const { return Component<TransformComponent>::TypeName(); };
		const uint32_t ID() const { return Component<TransformComponent>::GetID(); };
	};
}