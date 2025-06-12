
//------------------------------------------------------------------------------
//
// File Name:	Camera.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Camera.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    Camera::Frustum Camera::Frustum::FromCamera(glm::vec3 position, glm::vec3 up, glm::vec3 right, glm::vec3 front, float aspect, float fovY, float zNear, float zFar)
    {
        Frustum frustum;

        const float halfVSide = zFar * 0.75f * tanf(fovY * 0.5f);
        const float halfHSide = halfVSide * aspect;
        const glm::vec3 frontMultFar = zFar * front;

        frustum.planes[0] = { position + zNear * front, front };                        // Near
        frustum.planes[1] = { position + frontMultFar,      -front };                       // Far
        frustum.planes[2] = { position, glm::normalize(glm::cross(frontMultFar - right * halfHSide, up)) };       // right
        frustum.planes[3] = { position, glm::normalize(glm::cross(up, frontMultFar + right * halfHSide)) };       // Left
        frustum.planes[4] = { position, glm::normalize(glm::cross(right, frontMultFar - up * halfVSide)) };       // Top
        frustum.planes[5] = { position, glm::normalize(glm::cross(frontMultFar + up * halfVSide, right)) };       // Bottom

        return frustum;
    }
}
