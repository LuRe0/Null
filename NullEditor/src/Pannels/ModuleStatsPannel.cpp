
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
#include "ModuleStatsPannel.h"
#include "Null/Engine/Submodules/Scene.h"
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>

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
	//void ModuleStatsPannel::SetContext(Scene* scene)
	//{
	//	m_Context = scene;
	//}

	void ModuleStatsPannel::OnImGUIRender()
	{
		const auto& modules = NEngine::Instance().GetModules();

		ImGui::Begin("Module Stats");

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;
		flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		for (const auto& mod : modules)
		{


			if (mod.first->HasRenderImGui())
			{
				bool opened = ImGui::TreeNodeEx(mod.second.data(), flags);
				if (opened)
				{
					mod.first->RenderImGui();

					ImGui::TreePop();

				}

			}
		}


		ImGui::End();
	}
}
