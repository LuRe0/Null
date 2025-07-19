#pragma once
#include "IComponent.h"

namespace NULLENGINE
{

	struct TagComponent
	{
		static constexpr uint32_t MaxTags = 8;
		std::array<uint32_t, MaxTags> m_TagIDs = {};
		uint8_t m_TagCount = 0;
		ComponentFlagSet m_ComponentFlags;
	};

}