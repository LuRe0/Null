#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
	struct ParentComponent
	{
		uint32_t m_Parent;  // ID of the parent entity
		ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled);
	};

	struct ChildrenComponent
	{
		std::vector<uint32_t> m_Children;  // IDs of child entities
		ComponentFlagSet componentFlags = ComponentFlagSet(ComponentFlags_Enabled);
	};
}