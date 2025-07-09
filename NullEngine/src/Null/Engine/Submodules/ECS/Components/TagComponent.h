#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
	struct TagComponent : public BaseComponent
	{
		std::set<std::string> m_Tags;

		TagComponent(std::set<std::string> tags = std::set<std::string>())
			: m_Tags(tags)
		{}

		const std::string Name() const { return Component<TagComponent>::TypeName(); };
		const uint32_t ID() const { return Component<TagComponent>::GetID(); };

	};

}