
//------------------------------------------------------------------------------
//
// File Name:	CameraShake.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "CameraShake.h"
#include <glm/glm.hpp>




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	glm::vec2 CameraShake::GetOffset(float deltaTime)
    {
        elapsed += deltaTime;
        if (elapsed >= duration)
        {
            finished = true;
            return glm::vec2(0.0f);
        }

        float t = elapsed / duration;
        float strength = magnitude * (1.0f - Easing::Ease(t, easing)); // falloff over time

        float noiseX = std::sin(t * frequency * 2.0f * glm::pi<float>());
        float noiseY = std::cos(t * frequency * 2.0f * glm::pi<float>());

        glm::vec2 jitter = glm::vec2(noiseX, noiseY);

        if (glm::length(direction) > 0.0f)
            jitter = glm::normalize(direction) * noiseX;

        return jitter * strength;
    }

}
