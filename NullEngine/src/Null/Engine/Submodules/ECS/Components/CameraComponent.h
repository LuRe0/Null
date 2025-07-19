#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
	class Camera;

    struct CameraComponent
    {
        uint8_t m_CameraID = 0;
        uint8_t m_TypeID = 0;
        ComponentFlagSet m_ComponentFlags;
        bool m_IsMainCamera = true;
    };

}