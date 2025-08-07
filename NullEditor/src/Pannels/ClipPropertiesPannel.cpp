
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
#include "ClipPropertiesPannel.h"
#include "Null/Engine/Submodules/Scene.h"
//#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>
#include "../Editors/SceneEditor.h"
#include "../../../NullEngine/src/Null/Tools/ImGuiH.h"
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
	//void ClipPropertiesPannel::SetContext(Scene* scene)
	//{
	//	m_Context = scene;
	//}

	void ClipPropertiesPannel::OnImGUIRender()
	{


		ImGui::Begin("Clip Properties");

		if (m_PannelData->workingClip.frameCount > 0)  // Changed from startFrame >= 0
		{
			auto& clip = m_PannelData->workingClip;

			// Clip Name
			ImGui::Text("Clip Name:");
			ImGui::InputText("##ClipName", m_ClipNameBuffer, sizeof(m_ClipNameBuffer));
			if (strlen(m_ClipNameBuffer) > 0) {
				clip.nameID = STRID(m_ClipNameBuffer);
				m_PannelData->hasUnsavedChanges = true;
			}
			ImGui::Separator();

			// Frame Duration
			ImGui::Text("Timing:");
			if (ImGui::InputFloat("Anim Length", &m_PannelData->animLength, 0.01f, 0.1f, "%.3f"))
			{
				clip.frameDuration = std::max(0.001f, clip.frameDuration);
				m_PannelData->hasUnsavedChanges = true;
			}
			ImGui::Separator();

			// Animation Flags
			ImGui::Text("Flags:");

			bool isLooping = clip.flags.IsSet(AnimationFlags_IsLooping);
			if (ImGui::Checkbox("Loop", &isLooping))
			{
				clip.flags.Set(AnimationFlags_IsLooping, isLooping);

				m_PannelData->hasUnsavedChanges = true;
			}

			bool isReversed = clip.flags.IsSet(AnimationFlags_IsReversed);  // Assuming you have this flag
			if (ImGui::Checkbox("Reverse", &isReversed))
			{
				clip.flags.Set(AnimationFlags_IsReversed, isReversed);

				m_PannelData->hasUnsavedChanges = true;
			}

			bool isPingPong = clip.flags.IsSet(AnimationFlags_IsPingPong);  // Assuming you have this flag
			if (ImGui::Checkbox("Ping-Pong", &isPingPong))
			{
				clip.flags.Set(AnimationFlags_IsPingPong, isPingPong);

				m_PannelData->hasUnsavedChanges = true;
			}

			bool preserveFrame = clip.flags.IsSet(AnimationFlags_PreserveFrame);
			if (ImGui::Checkbox("Preserve Frame", &preserveFrame))
			{
				clip.flags.Set(AnimationFlags_PreserveFrame, preserveFrame);

				m_PannelData->hasUnsavedChanges = true;
			}

			ImGui::Separator();

			// Display clip info (derived from working clip)
			int endFrame = clip.startingFrame + clip.frameCount - 1;
			ImGui::SetWindowFontScale(1.250f);
			ImGui::Text("Selected Frames: %d - %d", clip.startingFrame, endFrame);
			ImGui::Text("Frame Count: %d", clip.frameCount);
			ImGui::Text("Total Duration: %.3f seconds", clip.frameCount * clip.frameDuration);

			// Preview the AnimationClip struct
			ImGui::Separator();
			ImGui::Text("Preview AnimationClip:");
			ImGui::Text("nameID: %u (from \"%s\")", clip.nameID, m_ClipNameBuffer);
			ImGui::Text("spriteSheetID: %u (from \"%s\")", clip.spriteSheetID, STRFROM(clip.spriteSheetID).c_str());
			ImGui::Text("startingFrame: %d", clip.startingFrame);
			ImGui::Text("frameCount: %d", clip.frameCount);
			ImGui::Text("frameDuration: %.3f", clip.frameDuration);

			// Display flags in a readable way
			ImGui::Text("Flags:");
			ImGui::BulletText("Loop: %s", (clip.flags.IsSet(AnimationFlags_IsLooping)) ? "true" : "false");
			ImGui::BulletText("Ping-Pong: %s", (clip.flags.IsSet(AnimationFlags_IsPingPong)) ? "true" : "false");
			ImGui::BulletText("Reversed: %s", (clip.flags.IsSet(AnimationFlags_IsReversed)) ? "true" : "false");
			ImGui::BulletText("Preserve Frame: %s", (clip.flags.IsSet(AnimationFlags_PreserveFrame)) ? "true" : "false");

			ImGui::SetWindowFontScale(1.0f);
			ImGui::Separator();

			// Save button with validation
			bool canSave = strlen(m_ClipNameBuffer) > 0 && clip.frameCount > 0;
			if (!canSave) {
				ImGui::BeginDisabled();
			}

			if (ImGui::Button("Create & Save Clip", ImVec2(-1, 0)))
			{
				NAnimationClipManager::Instance()->SaveClipToFile(clip);
				m_PannelData->hasUnsavedChanges = false;
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "Clip saved successfully!");
			}

			if (!canSave)
			{
				ImGui::EndDisabled();
				if (strlen(m_ClipNameBuffer) == 0) {
					ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "Enter a clip name to save");
				}
			}

			// Show unsaved changes indicator
			if (m_PannelData->hasUnsavedChanges)
			{
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "*");
				if (ImGui::IsItemHovered()) {
					ImGui::SetTooltip("Unsaved changes");
				}
			}
		}
		else
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "No frames selected");
			ImGui::Text("Select frames in the Frame Selection panel");
		}

		ImGui::End();
	}
}


