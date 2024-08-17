#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
	struct ParentComponent : public BaseComponent
	{
		uint32_t m_Parent;  // ID of the parent entity

		ParentComponent(uint32_t parent = 0)
			: m_Parent(parent)
		{}

		const std::string Name() const { return Component<ParentComponent>::TypeName(); };
		const uint32_t ID() const { return Component<ParentComponent>::GetID(); };
	};

	struct ChildrenComponent : public BaseComponent
	{
		std::vector<uint32_t> m_Children;  // IDs of child entities


		ChildrenComponent(const std::vector<uint32_t> children = std::vector<uint32_t>())
			: m_Children(children)
		{}


		const std::string Name() const { return Component<ChildrenComponent>::TypeName(); };
		const uint32_t ID() const { return Component<ChildrenComponent>::GetID(); };
	};
}