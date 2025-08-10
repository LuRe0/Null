
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
#include "../editors/AnimationClipEditor.h"
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
			if(ImGui::InputText("##ClipName", m_ClipNameBuffer, sizeof(m_ClipNameBuffer)))\
			{
				if (strlen(m_ClipNameBuffer) > 0) {
					clip.nameID = STRID(m_ClipNameBuffer);
					m_Parent->SetDirty(true);
				}
			}

			ImGui::Dummy({ 0,10 });

			ImGui::Separator();

			ImGui::Dummy({ 0,10 });


			// Frame Duration
	
			ImGui::CollapsingHeader("Animation Settings", ImGuiTreeNodeFlags_DefaultOpen);
			{
				if (ImGui::InputFloat("Anim Length", &m_PannelData->animLength, 0.01f, 0.1f, "%.3f"))
				{
					m_PannelData->animLength = std::max(0.001f, m_PannelData->animLength);
					m_Parent->SetDirty(true);
				}
			}
			clip.animationLength = m_PannelData->animLength;


			ImGui::Dummy({ 0,10 });

			ImGui::Separator();

			ImGui::Dummy({ 0,10 });

			// Animation Flags
			ImGui::CollapsingHeader("Animation F;ags", ImGuiTreeNodeFlags_DefaultOpen);
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

				bool preserveFrame = clip.flags.IsSet(AnimationFlags_PreserveFrame);
				if (ImGui::Checkbox("Preserve Frame", &preserveFrame))
				{
					clip.flags.Set(AnimationFlags_PreserveFrame, preserveFrame);

					m_Parent->SetDirty(true);
				}
			}

			ImGui::Dummy({ 0,10 });
			ImGui::Separator();
			ImGui::Dummy({ 0,10 });

			//ImGui::CollapsingHeader("Montage Preview", ImGuiTreeNodeFlags_DefaultOpen);
			//{
			//	// Display clip info (derived from working clip)
			//	int endFrame = clip.startingFrame + clip.frameCount - 1;

			//	ImGui::TextColored({1,1,0,1}, "Selected Frames: %d - %d", clip.startingFrame, endFrame);
			//	ImGui::TextColored({1,1,0,1}, "Frame Count: %d", clip.frameCount);
			//	ImGui::TextColored({1,1,0,1}, "Total Duration: %.3f seconds", clip.animationLength);
			//	ImGui::TextColored({1,1,0,1}, "nameID: %u (from \"%s\")", clip.nameID, m_ClipNameBuffer);
			//	ImGui::TextColored({1,1,0,1}, "spriteSheetID: %u (from \"%s\")", clip.spriteSheetID, STRFROM(clip.spriteSheetID).c_str());
			//	ImGui::TextColored({1,1,0,1}, "startingFrame: %d", clip.startingFrame);
			//	ImGui::TextColored({1,1,0,1}, "frameCount: %d", clip.frameCount);
			//	ImGui::TextColored({1,1,0,1}, "frameDuration: %.3f", clip.animationLength / clip.frameCount);

			//	// Display flags in a readable way
			//	ImGui::TextColored({ 1,1,0,1 }, "Animation Flags:");
			//	ImGui::BulletText("Loop: %s", (clip.flags.IsSet(AnimationFlags_IsLooping)) ? "true" : "false");
			//	ImGui::BulletText("Ping-Pong: %s", (clip.flags.IsSet(AnimationFlags_IsPingPong)) ? "true" : "false");
			//	ImGui::BulletText("Reversed: %s", (clip.flags.IsSet(AnimationFlags_IsReversed)) ? "true" : "false");
			//	ImGui::BulletText("Preserve Frame: %s", (clip.flags.IsSet(AnimationFlags_PreserveFrame)) ? "true" : "false");

			//}

			//ImGui::Dummy({ 0,10 });

			//ImGui::Separator();

			//ImGui::Dummy({ 0,10 });


			// Save button with validation
			bool canSave = strlen(m_ClipNameBuffer) > 0 && clip.frameCount > 0;
			if (!canSave) 
			{
				ImGui::BeginDisabled();
			}

			if (ImGui::Button("Create & Save Clip", ImVec2(-1, 0)))
			{
				m_Parent->SaveChanges();
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "Clip saved successfully!");
			}

			if (!canSave)
			{
				ImGui::EndDisabled();
				if (strlen(m_ClipNameBuffer) == 0) {
					ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "Enter a clip name to save");
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


