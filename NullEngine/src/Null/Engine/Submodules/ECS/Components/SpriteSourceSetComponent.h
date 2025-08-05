#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
    struct SpriteSourceSetComponent
    {
		uint32_t currentSourceID = -1; // Index in the system-wide storage for sprite sources
		uint32_t runtimeID = -1; // Unique ID for runtime identification
        ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled | ComponentFlags_Serialized);
    };
}