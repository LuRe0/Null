#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
	struct LifetimeComponent : public BaseComponent
	{
		float timeRemaining = 0.0f;

		LifetimeComponent(float inLifetime = 1.0f)
			: timeRemaining(inLifetime)
		{}

		const std::string Name() const { return Component<LifetimeComponent>::TypeName(); };
		const uint32_t ID() const { return Component<LifetimeComponent>::GetID(); };

	};

}