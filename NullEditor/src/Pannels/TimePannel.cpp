
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
#include "TimePannel.h"
#include "Null/Engine/Submodules/Scene.h"
//#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>
#include "../Editors/SceneEditor.h"
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
	//void TimePannel::SetContext(Scene* scene)
	//{
	//	m_Context = scene;
	//}

	void TimePannel::OnImGUIRender()
	{

		ImGui::Begin("Time Stats");

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;
		flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		int fps = Time::FPS();

		ImVec4 color;
		if (fps < 25)
			color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);       // Red
		else if (fps < 45)
			color = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);       // Orange
		else if (fps < 55)
			color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);       // Yellow
		else
			color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);       // Green

		ImGui::Text("Delta Time: %.4f s", Time::DeltaTime());
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::Text("FPS: %d", fps);
		ImGui::PopStyleColor();

		ImGui::End();
	}
}
