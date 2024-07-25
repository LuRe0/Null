
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
#include "SceneHierarchyPannel.h"
#include "Null/Engine/Submodules/Scene.h"
#include "imgui.h"
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
	void SceneHierarchyPannel::SetContext(Scene* scene)
	{
		m_Context = scene;
	}

	void SceneHierarchyPannel::OnImGUIRender()
	{
		ImGui::Begin("Scene Hierarchy");

		auto& entities = m_Context->GetManagedEntities();

		for (auto& entity : entities)
		{
			ImGuiTreeNodeFlags flags = (m_SelectedEntity == entity.GetID() ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)(entity.GetID()), flags, entity.GetName().c_str());

			if (ImGui::IsItemClicked())
			{
				m_SelectedEntity = entity.GetID();
			}

			if (opened)
			{
				ImGui::TreePop();

			}
		}

		ImGui::End();
	}
}
