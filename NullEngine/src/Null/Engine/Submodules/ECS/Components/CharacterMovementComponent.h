#pragma once
#include "IComponent.h"

namespace NULLENGINE
{

    enum CharacterMovementFlags : uint8_t
    {
        CharacterMovementFlags_IsGrounded = 1 << 0,
        CharacterMovementFlags_CanJump = 1 << 1,
        CharacterMovementFlags_JumpRequested = 1 << 2,
        CharacterMovementFlags_RotateToVelocity = 1 << 3,
        // add more flags here as needed
    };

    DEFINE_FLAG_SET(CharacterMovementFlagSet,
        uint8_t m_IsGrounded : 1;
    uint8_t m_CanJump : 1;
    uint8_t m_JumpRequested : 1;
    uint8_t m_Reserved : 5;
        );


    struct CharacterMovementComponent
    {
        glm::vec2 moveInput = glm::vec2(0.0f); // Script sets this [-1, 1]
		glm::vec2 angularInput = glm::vec2(0.0f); // Script sets this [-1, 1]
        float maxMoveSpeed = 5.0f;       // Clamp velocity to this
        float acceleration = 100.0f;     // Force applied toward target direction
		float maxAngularSpeed = 5.0f; // Clamp angular velocity to this
		float angularAcceleration = 100.0f; // Force applied toward target rotation
        float jumpImpulse = 10.0f;


        CharacterMovementFlagSet flags = CharacterMovementFlagSet(CharacterMovementFlags_IsGrounded);

        ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled | ComponentFlags_Serialized);
    };
}