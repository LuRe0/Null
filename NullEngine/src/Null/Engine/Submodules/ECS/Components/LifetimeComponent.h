#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
	struct LifetimeComponent
	{
		float timeRemaining = 0.0f;
		ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled | ComponentFlags_Serialized);
	};

}