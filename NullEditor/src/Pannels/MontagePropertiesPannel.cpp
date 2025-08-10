
//------------------------------------------------------------------------------
//
// File Name:	NStub.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "MontagePropertiesPannel.h"
#include "Null/Engine/Submodules/Scene.h"
//#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>
#include "../Editors/AnimationMontageEditor.h"
#include "NIncludes.h"

//#include "backends/imgui_impl_opengl3.h"
//#include "backends/imgui_impl_glfw.h"

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void MontagePropertiesPannel::OnImGUIRender()
	{

		ImGui::Begin("Montage Properties");

		if (!m_PannelData->selectedClip)
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "No animation clip selected");
			ImGui::End();
			return;
		}

		AnimationClip& clip = *m_PannelData->selectedClip;

		ImGui::SetWindowFontScale(1.250f);
		ImGui::TextColored({1,1,1,1}, ("Montage: " + STRFROM(clip.nameID)).c_str());
		ImGui::SetWindowFontScale(1.0f);

		ImGui::Dummy({ 0,10 });

		ImGui::Separator();

		ImGui::Dummy({ 0,10 });

		// Frame Duration
		if (ImGui::InputFloat("Anim Length", &clip.animationLength, 0.01f, 0.1f, "%.3f"))
		{
			clip.animationLength = std::max(0.001f, clip.animationLength);
			m_Parent->SetDirty(true);
		}

		ImGui::Dummy({ 0,10 });

		ImGui::Separator();

		ImGui::Dummy({ 0,10 });

		// Animation Flags
		//ImGui::Text("Flags:");

		if (ImGui::CollapsingHeader("Animation Flags", ImGuiTreeNodeFlags_DefaultOpen))
		{

			bool isLooping = clip.flags.IsSet(AnimationFlags_IsLooping);
			if (ImGui::Checkbox("Loop", &isLooping))
			{
				clip.flags.Set(AnimationFlags_IsLooping, isLooping);

				m_Parent->SetDirty(true);
			}

			bool isReversed = clip.flags.IsSet(AnimationFlags_IsReversed);  // Assuming you have this flag
			if (ImGui::Checkbox("Reverse", &isReversed))
			{
				clip.flags.Set(AnimationFlags_IsReversed, isReversed);

				m_Parent->SetDirty(true);
			}

			bool isPingPong = clip.flags.IsSet(AnimationFlags_IsPingPong);  // Assuming you have this flag
			if (ImGui::Checkbox("Ping-Pong", &isPingPong))
			{
				clip.flags.Set(AnimationFlags_IsPingPong, isPingPong);

				m_Parent->SetDirty(true);
			}
		}
		ImGui::Separator();

		if (ImGui::CollapsingHeader("SpriteSource", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (m_PannelData->selectedSpriteSource)
			{
				ImGui::SetWindowFontScale(1.250f);
				ImGui::TextColored({1,1,0,1}, ("Texture: "  + m_PannelData->selectedSpriteSource->GetName()).c_str());
				ImGui::SetWindowFontScale(1.0f);
			}
		

			ImGui::Dummy({0,10});

			ImGui::Image((ImTextureID)m_PannelData->selectedSpriteSource->GetTexture()->GetID(), ImVec2(256, 256),
				ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));

		}
		ImGui::End();
	}
}
