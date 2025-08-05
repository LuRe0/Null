#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
    struct DestroyedComponent
    {
        // Add your component data below
        ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled);
    };
}