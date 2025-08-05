#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
    struct NameComponent
    {
		uint32_t nameID = 0; // Unique identifier for the name
        ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled | ComponentFlags_Serialized);
    };
}