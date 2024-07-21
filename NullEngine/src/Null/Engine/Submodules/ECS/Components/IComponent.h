#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NStub.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//

class b2Body;
class b2Fixture;

namespace NULLENGINE
{
	class NLE_API IComponent
	{
	protected:
		static uint32_t m_NextID;
	};

	template <typename T>
	class NLE_API Component : public IComponent
	{
	public:
	
		static uint32_t GetID()
		{
			static uint32_t id = ++m_NextID;
			return id;
		}

		
		static std::string TypeName() {
			std::string fullName = typeid(T).name();
			// Manual parsing: adjust based on compiler output
			auto pos = fullName.find_last_of(':');
			return fullName.substr(pos + 1);
		}
	private:
	};

	struct BaseComponent 
	{
		virtual ~BaseComponent() = default;
		virtual const std::string Name() = 0;
		BaseComponent() = default;
	};

	struct  TransformComponent : public BaseComponent
	{
		glm::vec3 m_Translation;
		glm::vec3 m_Scale;
		glm::vec3 m_Rotation;
		bool m_Dirty;

		glm::mat4 m_TransformMatrix;


		// Constructor with default values
		TransformComponent(glm::vec3 translation = glm::vec3(0.0f),
			glm::vec3 scale = glm::vec3(1.0f),
			glm::vec3 rotation = glm::vec3(0.0f),
			bool dirty = false)
			: m_Translation(translation),
			m_Scale(scale),
			m_Rotation(rotation),
			m_Dirty(dirty),
			m_TransformMatrix(glm::mat4(1.0f)) // Identity matrix by default
		{}

		const std::string Name() { return Component<TransformComponent>::TypeName(); };
	};

	struct  Rigidbody2DComponent : public BaseComponent
	{
		enum BodyType
		{
			Static = 0,
			Kinematic,
			Dynamic,
		};

		BodyType m_Type;
		bool m_FixedRotation;
		b2Body* m_RuntimeBody;

		// Constructor with default values
		Rigidbody2DComponent(BodyType type = Static,
			bool fixedRotation = true,
			b2Body* runtimeBody = nullptr)
			: m_Type(type),
			m_FixedRotation(fixedRotation),
			m_RuntimeBody(runtimeBody)
		{}

		const std::string Name() { return Component<Rigidbody2DComponent>::TypeName(); };

	};

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
			glm::vec2 scale = { 0.0f, 0.0f },
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

	class SpriteSource;  // Forward declaration of Texture
	class Mesh;  // Forward declaration of Texture

	struct SpriteComponent : public BaseComponent
	{
		unsigned int m_FrameIndex;
		SpriteSource* m_SpriteSource;
		Mesh* m_Mesh;
		glm::vec4 m_Color;
		std::string m_ShaderName;

		// Constructor with default values
		SpriteComponent(unsigned int frameIndex = 0,
			SpriteSource* spriteSource = nullptr,
			Mesh* mesh = nullptr,
			glm::vec4 color = glm::vec4(1.0f),
			std::string shaderName = "default")
			: m_FrameIndex(frameIndex),
			m_SpriteSource(spriteSource),
			m_Mesh(mesh),
			m_Color(color),
			m_ShaderName(shaderName)
		{}

		const std::string Name() { return Component<SpriteComponent>::TypeName(); };

	};
}