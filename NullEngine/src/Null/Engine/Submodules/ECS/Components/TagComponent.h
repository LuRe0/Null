#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
	struct TagComponent : public BaseComponent
	{
		std::string m_Tag;

		TagComponent(std::string tag = "")
			: m_Tag(tag)
		{}

		const std::string Name() const { return Component<TagComponent>::TypeName(); };
		const uint32_t ID() const { return Component<TagComponent>::GetID(); };

	};

}