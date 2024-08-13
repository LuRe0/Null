#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
	class SpriteSource;  // Forward declaration of Texture
	class Mesh;  // Forward declaration of Texture

	struct SpriteComponent : public BaseComponent
	{
		unsigned int m_FrameIndex;
		SpriteSource* m_SpriteSource;
		Mesh* m_Mesh;
		glm::vec4 m_Color;
		std::string m_ShaderName;
		float m_Thickness;
		float m_Fade;

		// Constructor with default values
		SpriteComponent(unsigned int frameIndex = 0,
			SpriteSource* spriteSource = nullptr,
			Mesh* mesh = nullptr,
			glm::vec4 color = glm::vec4(1.0f),
			std::string shaderName = "default", float thickness = 1.0f, float fade = 0.005f)
			: m_FrameIndex(frameIndex),
			m_SpriteSource(spriteSource),
			m_Mesh(mesh),
			m_Color(color),
			m_ShaderName(shaderName),
			m_Thickness(thickness),
			m_Fade(fade)
		{}

		const std::string Name() const { return Component<SpriteComponent>::TypeName(); };
		const uint32_t ID() const { return Component<SpriteComponent>::GetID(); };

	};

}