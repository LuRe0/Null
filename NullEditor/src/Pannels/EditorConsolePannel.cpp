
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
#include "EditorConsolePannel.h"
#include "Null/Engine/Submodules/Scene.h"
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>
#include "Null/Tools/ImguiSink.h"
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
	//void EditorConsolePannel::SetContext(Scene* scene)
	//{
	//	m_Context = scene;
	//}


	void EditorConsolePannel::OnImGUIRender()
	{
		ImGui::Begin("Console");

		//for (const auto& line : m_ConsoleLog)
		//{
		//	ImGui::Text("%s", line.c_str());
		//}
		auto& sink = Trace::GetImguiSink();
		sink->print();
		//Trace::GetImguiSink().get()->print();

		ImGui::End();

	/*	std::cout << "Deez";
		ImGui::Begin("Console");
		for (const auto& line : m_ConsoleLog) {
			ImGui::Text("%s", line.c_str());
		}

		ImGui::End();*/
		//bool b = true;
		//ImGui::ShowDebugLogWindow(&b);
	}
}
