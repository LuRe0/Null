
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



	

		ImGui::PushItemWidth(100);
		ImGui::Text("Scene Name: "); ImGui::SameLine();

		if (ImGui::InputText("##name", &m_PannelData->m_Context->m_Name))
		{
			if (m_PannelData->m_Context->m_Name.empty())
			{
				m_PannelData->m_Context->m_Name = "New Scene";
			}
		}
		ImGui::PopItemWidth();





		ImGui::BeginChild("##target");

		auto& entities = m_PannelData->m_Context->ManagedEntities();

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_PannelData->m_SelectedEntity = {};

		// Create component
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Create Entity"))
				m_PannelData->m_SelectedEntity = m_PannelData->m_Context->CreateEmptyEntity("New Entiy");


			if (ImGui::MenuItem("Load Archetype"))
			{
				const std::string& archetype = FileDialog::OpenFile("Null Engine archetype (*.ent)\0*.ent\0");
				if (!archetype.empty())
					m_PannelData->m_SelectedEntity = m_PannelData->m_Context->LoadArchetype(archetype);
			}
			ImGui::EndPopup();
		}



		for (auto& entity : entities)
		{

			if (entity.Has<ParentComponent>())
				continue;

			ImGuiTreeNodeFlags flags = (m_PannelData->m_SelectedEntity == entity.GetID() ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 100);
			std::string id = "ID:" + std::to_string(entity.GetID());
			ImGui::TextColored(ImVec4(0, 1, 0, 1), id.c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosX(0);
			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)(entity.GetID()), flags, entity.GetName().c_str());

			if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
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

				if (ImGui::MenuItem("Save as new Archetype"))
				{
					const std::string& archetype = FileDialog::SaveFile("Null Engine Archetype (*.ent)\0*.ent\0");
					if (!archetype.empty())
						m_PannelData->m_Context->SerializeArchetype(archetype, m_PannelData->m_SelectedEntity);
				}

				if (ImGui::MenuItem("Save Archetype"))
				{
					Entity& entity = m_PannelData->m_Context->GetEntity(m_PannelData->m_SelectedEntity);
					if (!entity.m_Archetype.empty())
						m_PannelData->m_Context->SerializeArchetype(entity.m_Archetype, m_PannelData->m_SelectedEntity);
				}

				ImGui::EndPopup();

			}

			if (opened)
			{
				if (entity.Has<ChildrenComponent>())
				{
					auto& cComp = entity.Get<ChildrenComponent>();
					auto& children = cComp.m_Children;

					for (const auto& childID : children)
					{
						auto& childEntity = m_PannelData->m_Context->GetEntity(childID);

						// Recursive call to draw the child entity and its children
						DrawEntityNode(childEntity);
					}
				}
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



		ImGui::EndChild();
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_FILE"))
			{
				std::string filename((const char*)payload->Data);

				if (!filename.empty())
					m_PannelData->m_SelectedEntity = m_PannelData->m_Context->LoadArchetype(filename);
			}
			ImGui::EndDragDropTarget();
		}


		ImGui::End();
	}
	void SceneHierarchyPannel::DrawEntityNode(Entity& entity)
	{
		ImGuiTreeNodeFlags flags = (m_PannelData->m_SelectedEntity == entity.GetID() ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

		ImVec2 cursorPos = ImGui::GetCursorPos(); // Save the current cursor position

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 100);
		std::string id = "ID:" + std::to_string(entity.GetID());
		ImGui::TextColored(ImVec4(0,1,0,1), id.c_str());
		ImGui::SameLine();

		ImGui::SetCursorPos(cursorPos);
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)(entity.GetID()), flags, entity.GetName().c_str());

		if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
		{
			m_PannelData->m_SelectedEntity = entity.GetID();
		}

		if (opened)
		{
			if (entity.Has<ChildrenComponent>())
			{
				auto& cComp = entity.Get<ChildrenComponent>();
				auto& children = cComp.m_Children;

				for (const auto& childID : children)
				{
					auto& childEntity = m_PannelData->m_Context->GetEntity(childID);

					// Recursive call to draw the child entity and its children
					DrawEntityNode(childEntity);
				}
			}
			ImGui::TreePop();
		}
	}
}
