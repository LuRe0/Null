
//------------------------------------------------------------------------------
//
// File Name:	ImGuiH.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "ImGuiH.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	std::tuple<bool, bool, bool> ImGuiH::CollapsingHeaderWithCheckboxAndRemove(const std::string& label, bool* enabled)
	{
		bool remove = false;
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_SpanAvailWidth;

		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImVec2 contentRegion = ImGui::GetContentRegionAvail();
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		ImGui::PushID(label.c_str());

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 0));
		std::string checkboxLabel = "##" + label + "_enabled";
		ImGui::Checkbox(checkboxLabel.c_str(), enabled);
		ImGui::SameLine();

		bool open = ImGui::TreeNodeEx(label.c_str(), flags);
		ImGui::PopStyleVar();

		ImGui::SameLine(contentRegion.x - lineHeight);
		ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + contentRegion.x - lineHeight, cursorPos.y));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0, 0, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));

		if (ImGui::Button(("X##" + label).c_str(), ImVec2(lineHeight, lineHeight)))
			remove = true;

		ImGui::PopStyleColor(3);
		ImGui::PopID();

		return { open, *enabled, remove };
	}


	std::tuple<bool, bool> ImGuiH::CollapsingHeaderWithRemove(const std::string& label)
	{
		bool remove = false;
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_SpanAvailWidth;

		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImVec2 contentRegion = ImGui::GetContentRegionAvail();
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		ImGui::PushID(label.c_str());

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 0));

		bool open = ImGui::TreeNodeEx(label.c_str(), flags);
		ImGui::PopStyleVar();

		ImGui::SameLine(contentRegion.x - lineHeight);
		ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + contentRegion.x - lineHeight, cursorPos.y));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0, 0, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));

		if (ImGui::Button(("X##" + label).c_str(), ImVec2(lineHeight, lineHeight)))
			remove = true;

		ImGui::PopStyleColor(3);
		ImGui::PopID();

		return { open, remove };
	}



	std::pair<bool, bool>  ImGuiH::DrawModifierHeader(const char* label, uint32_t& flags, uint32_t flagBit, int id)
	{
		bool enabled = (flags & flagBit) != 0;
		auto result = DrawModifierHeader(label, &enabled, id);

		if (enabled) flags |= flagBit;
		else         flags &= ~flagBit;

		return result;
	}

	std::pair<bool, bool> ImGuiH::DrawModifierHeader(const char* label, bool* enabled, int id)
	{
		// Only push ID around checkbox to isolate ID scope
		ImGui::PushID(id);
		std::string checkboxLabel = "##Enable_" + std::to_string(id);
		ImGui::Checkbox(checkboxLabel.c_str(), enabled);
		ImGui::SameLine();
		ImGui::PopID();

		ImVec4 textColor = *enabled ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);
		if (!*enabled)
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 0.4f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.4f, 0.4f, 0.4f, 0.3f));
		}

		bool opened = ImGui::TreeNodeEx(label,
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth);

		if (!*enabled) ImGui::PopStyleColor(4);
		ImGui::PopStyleColor();

		return { opened, *enabled };
	}

	std::pair<bool, bool> ImGuiH::DrawTopLevelHeader(const char* label, bool* enabled, int id)
	{
		ImGui::PushID(id);
		ImGui::Checkbox(("##" + std::string(label) + "_enabled").c_str(), enabled);
		ImGui::SameLine();

		// Use regular TreeNode (not framed)
		bool open = ImGui::TreeNode(label);

		ImGui::PopID();
		return { open, *enabled };
	}


	std::tuple<bool, bool, bool> ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove(
		const std::string& label,
		uint8_t& flags,
		uint32_t flagBit)
	{
		bool remove = false;

		ImGuiTreeNodeFlags treeFlags =
			ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_SpanAvailWidth;

		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImVec2 contentRegion = ImGui::GetContentRegionAvail();
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		ImGui::PushID(label.c_str());

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 0));
		std::string checkboxLabel = "##" + label + "_enabled";
		bool enabled = (flags & flagBit) != 0;
		if (ImGui::Checkbox(checkboxLabel.c_str(), &enabled))
		{
			if (enabled)
				flags |= flagBit;
			else
				flags &= ~flagBit;
		}

		ImGui::SameLine();

		ImVec4 textColor = enabled ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);
		if (!enabled)
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 0.4f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.4f, 0.4f, 0.4f, 0.3f));
		}


		bool open = ImGui::TreeNodeEx(label.c_str(), treeFlags);

		if (!enabled) ImGui::PopStyleColor(4);

		ImGui::PopStyleColor();

		ImGui::PopStyleVar();

		ImGui::SameLine(contentRegion.x - lineHeight);
		ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + contentRegion.x - lineHeight, cursorPos.y));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0, 0, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));

		if (ImGui::Button(("X##" + label).c_str(), ImVec2(lineHeight, lineHeight)))
			remove = true;

		ImGui::PopStyleColor(3);

		// Grey out / disable content if !enabled
		if (!enabled)
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

		if (!enabled) ImGui::PopStyleVar();

		ImGui::PopID();

		//if (!enabled) t;

		return { open, enabled, remove };
	}
}
