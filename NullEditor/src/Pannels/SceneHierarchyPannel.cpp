
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
#include "Null/Tools/FileDialog.h"
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>

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
	void SceneHierarchyPannel::OnImGUIRender()
	{
		ImGui::Begin("Scene Hierarchy");

		ImGui::Text("Scene Name: "); ImGui::SameLine();

		if (ImGui::InputText("##name", &m_PannelData->m_Context->m_Name))
		{
			if (m_PannelData->m_Context->m_Name.empty())
			{
				m_PannelData->m_Context->m_Name = "New Scene";
			}
		}

		auto& entities = m_PannelData->m_Context->GetManagedEntities();

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_PannelData->m_SelectedEntity = {};

		// Create component
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Create Entity"))
				m_PannelData->m_SelectedEntity = m_PannelData->m_Context->CreateEmptyEntity("New Entiy");


			if (ImGui::MenuItem("Load Archetype"))
			{
				const std::string& archetype = FileDialog::OpenFile("Null Engine archetype (*.json)\0*.json\0");
				if (!archetype.empty())
					m_PannelData->m_SelectedEntity = m_PannelData->m_Context->LoadArchetype(archetype);
			}
			ImGui::EndPopup();
		}



		for (auto& entity : entities)
		{
			ImGuiTreeNodeFlags flags = (m_PannelData->m_SelectedEntity == entity.GetID() ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)(entity.GetID()), flags, entity.GetName().c_str());

			if (ImGui::IsItemClicked())
			{
				m_PannelData->m_SelectedEntity = entity.GetID();
			}

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Delete Entity"))
				{
					m_PannelData->m_Context->DeleteEntity(entity.GetID());

					if (entity.GetID() == m_PannelData->m_SelectedEntity)
					{
						m_PannelData->m_SelectedEntity = {};
					}
				}

				if (ImGui::MenuItem("Save as Archetype"))
				{
					const std::string& archetype = FileDialog::SaveFile("Null Engine Archetype (*.json)\0*.json\0");
					if(!archetype.empty())
						m_PannelData->m_Context->SerializeArchetype(archetype, m_PannelData->m_SelectedEntity);
				}

				ImGui::EndPopup();

			}

			if (opened)
			{
				ImGui::TreePop();
			}


		}

		//bool isDeleted = false;
		//if (ImGui::BeginPopupContextItem())
		//{


		//}


		//if (isDeleted)
		//{

		//}


		ImGui::End();
	}
}
