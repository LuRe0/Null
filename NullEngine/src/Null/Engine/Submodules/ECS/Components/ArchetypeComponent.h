#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
    struct ArchetypeComponent
    {
        uint32_t archetypeID = 0; // Unique identifier for the name
        uint32_t parentArchetypeID = 0; // Unique identifier for the name
        ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled | ComponentFlags_Serialized);
        // Add your component data below
    };
}