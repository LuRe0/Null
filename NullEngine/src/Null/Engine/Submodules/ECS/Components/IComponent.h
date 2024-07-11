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

	struct NLE_API TransformComponent
	{
		glm::vec2 Translation;
		glm::vec2 Scale;
		float Rotation;
	};

	struct NLE_API Rigidbody2DComponent
	{
		float m_Mass = 1.0f;                // Mass of the rigidbody
		glm::vec2 m_OldTranslation;       // Velocity of the rigidbody
		glm::vec2 m_Velocity;       // Velocity of the rigidbody
		glm::vec2 m_Acceleration;       // Velocity of the rigidbody
		glm::vec2 m_Force;          // Force applied to the rigidbody
		float m_Drag = 0.0f;               // Linear drag applied to the rigidbody
		float m_GravityScale = 1.0f;      // Scale factor for gravity
	};

	struct Texture;  // Forward declaration of Texture

	struct SpriteComponent {
		Texture* m_Texture;   // Pointer to the texture to be used
		glm::vec4 m_Color;    // Color tint of the sprite
	};
}