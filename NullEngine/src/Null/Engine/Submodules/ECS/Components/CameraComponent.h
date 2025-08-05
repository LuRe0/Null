#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
	class Camera;

    struct CameraInitData
    {
        glm::vec3 position = { 0.0f, 0.0f, 0.0f }; // both 2D and 3D can use this
        float zoom = 1.0f;        // For 2D
        float fov = 60.0f;        // For 3D
    };


    struct CameraComponent
    {
        CameraInitData initData;
        uint8_t cameraID = 0;
        uint8_t typeID = 0;
        ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled | ComponentFlags_Serialized);
        bool mainCamera = true;
    };

}