#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	ImGuiH.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//


namespace NULLENGINE
{

	struct ScopedTree
	{
		bool open = false;

		ScopedTree(const char* label, ImGuiTreeNodeFlags flags)
		{
			open = ImGui::TreeNodeEx(label, flags);
		}

		~ScopedTree()
		{
			if (open)
				ImGui::TreePop();
		}

		operator bool() const { return open; }
	};


	static class NLE_API ImGuiH
	{
	public:
		static std::tuple<bool, bool, bool> CollapsingHeaderWithCheckboxAndRemove(const std::string& label, bool* enabled);

		static std::pair<bool, bool> DrawModifierHeader(const char* label, uint32_t& flags, uint32_t flagBit, int id);

		static std::pair<bool, bool> DrawModifierHeader(const char* label, bool* enabled, int id);

		static std::pair<bool, bool> DrawTopLevelHeader(const char* label, bool* enabled, int id);

		static std::tuple<bool, bool, bool> CollapsingHeaderWithFlagCheckboxAndRemove(const std::string& label, uint8_t& flags, uint32_t flagBit);

		template<typename Func>
		static void DrawModifierSection(const char* label, bool* enabled, int id, Func drawContents)
		{
			if (auto [open, enabledFlag] = ImGuiH::DrawModifierHeader(label, enabled, id); open)
			{
				if (enabledFlag)
					drawContents();
				ImGui::TreePop();
			}
		}
	};

}