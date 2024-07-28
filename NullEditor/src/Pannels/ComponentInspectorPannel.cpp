
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
#include "ComponentInspectorPannel.h"
#include "Null/Engine/Submodules/Scene.h"
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
	//void ComponentInspectorPannel::SetContext(Scene* scene)
	//{
	//	m_Context = scene;
	//}

	void ComponentInspectorPannel::OnImGUIRender()
	{
		ImGui::Begin("Component Inspector");

		if (!m_PannelData->m_SelectedEntity)
		{

			float availableWidth = ImGui::GetContentRegionAvail().x;
			float textWidth = ImGui::CalcTextSize("Warning: No Object Selected").x;
			float centerPosX = (availableWidth - textWidth) * 0.5f;
			ImGui::SetCursorPosX(centerPosX);

			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: No Object Selected");
		}
		else
		{
			// display name and update if changed
			Entity& selectedEntity = m_PannelData->m_Context->GetEntity(m_PannelData->m_SelectedEntity);

			ImGui::Text("Entity Name: "); ImGui::SameLine(); 

			if (ImGui::InputText("##name", &selectedEntity.m_Name))
			{
				if (selectedEntity.GetName().empty())
				{
					selectedEntity.SetName("Entity" + std::to_string(selectedEntity.GetID()));
				}
			}
		}

		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NComponentFactory* factory = NEngine::Instance().Get<NComponentFactory>();
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();
	
		if (m_PannelData->m_SelectedEntity)
		{
			const auto& entityComponentSignatures = registry->EntitySignature(m_PannelData->m_SelectedEntity);

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;
			flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

			for (size_t i = 0; i < entityComponentSignatures.size(); i++)
			{

				//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
				float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
				ImVec2 contentRegion = ImGui::GetContentRegionAvail();

				if (entityComponentSignatures.test(i))
				{
					BaseComponent& component = registry->GetComponent(m_PannelData->m_SelectedEntity, i);
					ImGui::Separator();

					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 0));
					std::string checkboxLabel = "##Enable Component" + std::to_string(i);
					ImGui::Checkbox(checkboxLabel.c_str(), &component.m_Enabled);

					ImGui::SameLine();

					bool opened = ImGui::TreeNodeEx(component.Name().data(), flags);

					ImGui::PopStyleVar();

					float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
					ImVec2 contentRegion = ImGui::GetContentRegionAvail();
					ImGui::SameLine(contentRegion.x + lineHeight*.55f);
					bool removed = false;
			
					// Push the style color for the button
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.5f, 1.0f)); 
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); 

					if (ImGui::Button("X", ImVec2{ lineHeight, lineHeight }))
					{
						removed = true;
					}

					ImGui::PopStyleColor(3);
					if (opened)
					{
						factory->ViewComponent(Entity(m_PannelData->m_SelectedEntity, registry), i);
						ImGui::TreePop();
					}

					if (removed)
					{
						eventManager->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(m_PannelData->m_SelectedEntity, i));
					}
				}
			}

			ImGui::Separator();


			ImVec2 cursorPos = ImGui::GetCursorPos();
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 buttonSize = ImVec2(150, 25); 
			ImVec2 buttonPos = ImVec2(cursorPos.x + (windowSize.x - buttonSize.x) * 0.5f, cursorPos.y);
			ImGui::SetCursorPos(buttonPos);

			if (ImGui::Button("Add Component", buttonSize))
				ImGui::OpenPopup("AddComponent");

			if (ImGui::BeginPopup("AddComponent"))
			{
				const auto& componentsNames = factory->GetComponentNames();

				for (const auto& name : componentsNames)
				{
					if (!registry->HasComponent(m_PannelData->m_SelectedEntity, factory->GetComponentID(name)))
					{
						if (ImGui::MenuItem(name.c_str()))
						{
							eventManager->QueueEvent(std::make_unique<EntityAddComponentEvent>(m_PannelData->m_SelectedEntity, factory->GetComponentID(name)));

							factory->CreateUniqueComponent(name, JSON(), registry, m_PannelData->m_SelectedEntity);
							ImGui::CloseCurrentPopup();
						}
					}
				}

				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}
}
