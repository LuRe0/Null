#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
    struct DoNotSerializeComponent
    {
        ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled);

        // Add your component data below
    };
}