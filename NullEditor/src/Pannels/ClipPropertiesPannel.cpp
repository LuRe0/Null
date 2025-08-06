
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


		if (m_PannelData->startFrame >= 0)
		{
			// Clip Name
			ImGui::Text("Clip Name:");
			ImGui::InputText("##ClipName", m_ClipNameBuffer, sizeof(m_ClipNameBuffer));

			ImGui::Separator();

			// Frame Duration
			ImGui::Text("Timing:");
			ImGui::InputFloat("Frame Duration", &m_FrameDuration, 0.01f, 0.1f, "%.3f");
			m_FrameDuration = std::max(0.001f, m_FrameDuration);

			ImGui::Separator();

			// Animation Flags
			ImGui::Text("Flags:");

			ImGui::Checkbox("Loop", &m_IsLooping);

			ImGui::Checkbox("Reverse", &m_Reversed);

			ImGui::Checkbox("Ping-Pong", &m_PingPong);

			ImGui::Checkbox("Preserve Frame", &m_PreserveFrame);


			ImGui::Separator();

			// Current selection info


			int start, end, count;
			start = m_PannelData->startFrame;
			end = m_PannelData->endFrame;
			count = end - start + 1;

			ImGui::SetWindowFontScale(1.250f);
			ImGui::Text("Selected Frames: %d - %d", start, end);
			ImGui::Text("Frame Count: %d", count);
			ImGui::Text("Total Duration: %.3f seconds", count * m_FrameDuration);

			// Preview the AnimationClip struct that would be created
			ImGui::Separator();
			ImGui::Text("Preview AnimationClip:");
			ImGui::Text("nameID: %u (from \"%s\")", STRID(m_ClipNameBuffer), m_ClipNameBuffer);
			ImGui::Text("spriteSheetID: %u", m_PannelData->selectedSpriteSourceID);
			ImGui::Text("startingFrame: %d", start);
			ImGui::Text("frameCount: %d", end);
			ImGui::Text("frameDuration: %.3f", m_FrameDuration);
			ImGui::Text("Loop: %s", m_IsLooping ? "true" : "false");
			ImGui::Text("Ping-Pong: %s", m_PingPong ? "true" : "false");
			ImGui::Text("Reversed: %s", m_Reversed ? "true" : "false");
			ImGui::Text("Preserve Frame: %s", m_PreserveFrame ? "true" : "false");
			ImGui::SetWindowFontScale(1.0f);

			ImGui::Separator();

			if (ImGui::Button("Create & Save Clip", ImVec2(-1, 0)))
			{
			}


			ImGui::Separator();

			//	// Load existing clip section
			//	ImGui::Text("Load Existing Clip:");
			//	if (ImGui::Button("Browse Clips", ImVec2(-1, 0)))
			//	{
			//	}
		}
		else
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "No frames selected");
			ImGui::Text("Select frames in the Frame Selection panel");
		}

		ImGui::End();
	}
}


