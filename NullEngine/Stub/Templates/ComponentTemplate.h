#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
    struct {{COMPONENT_NAME}}
    {
        ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled | ComponentFlags_Serialized);

        // Add your component data below
    };
}