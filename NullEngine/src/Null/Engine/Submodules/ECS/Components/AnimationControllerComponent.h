#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
    struct AnimationControllerComponent
    {
        uint32_t stateMachineID;  // Points to shared resource
        uint32_t currentStateID;  // Instance state
        ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled | ComponentFlags_Serialized);
    };
}