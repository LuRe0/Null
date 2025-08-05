#pragma once
#include "IComponent.h"

namespace NULLENGINE
{

	struct TagComponent
	{
		static constexpr uint32_t MaxTags = 8;
		std::array<uint32_t, MaxTags> tagIDs = {};
		uint8_t tagCount = 0;
		ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled | ComponentFlags_Serialized);
	};

}