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

	struct  TransformComponent
	{
		glm::vec3 m_Translation;
		glm::vec3 m_Scale;
		glm::vec3 m_Rotation;
		bool m_Dirty;

		// Padding to ensure proper alignment of m_TransformMatrix
		char padding[3];

		glm::mat4 m_TransformMatrix;
	};

	struct  Rigidbody2DComponent
	{
		float m_Mass = 1.0f;                // Mass of the rigidbody
		glm::vec3 m_OldTranslation;       // Velocity of the rigidbody
		glm::vec3 m_Velocity;       // Velocity of the rigidbody
		glm::vec3 m_Acceleration;       // Velocity of the rigidbody
		glm::vec3 m_Force;          // Force applied to the rigidbody
		float m_Drag = 0.0f;               // Linear drag applied to the rigidbody
		float m_GravityScale = 0;      // Scale factor for gravity
	};

	class SpriteSource;  // Forward declaration of Texture
	class Mesh;  // Forward declaration of Texture

	struct SpriteComponent 
	{
		unsigned int m_FrameIndex = 0;

		SpriteSource* m_SpriteSource;   // Pointer to the texture to be used

		Mesh* m_Mesh;   // Pointer to the mesh to be used

		glm::vec4 m_Color;    // Color tint of the sprite

		std::string m_ShaderName;
	};
}