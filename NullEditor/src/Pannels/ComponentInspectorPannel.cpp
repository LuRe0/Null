
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
//#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
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
		else if (m_PannelData->m_Context->HasEntity(m_PannelData->m_SelectedEntity))
		{
			// display name and update if changed
			Entity& selectedEntity = m_PannelData->m_Context->GetEntity(m_PannelData->m_SelectedEntity);

			ImGui::Text("Entity Name: "); ImGui::SameLine();
			auto& nameComp = selectedEntity.Get<NameComponent>();
			std::string name = STRFROM(nameComp.nameID);

			ImGui::InputText("##name", &name);

			if(ImGui::IsItemDeactivatedAfterEdit())
			{
				if (name.empty())
				{
					name = ("Entity" + std::to_string(selectedEntity.GetID()));
				}

				nameComp.nameID = STRID(name);
			}
		}

		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NComponentFactory* factory = NEngine::Instance().Get<NComponentFactory>();
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		if (m_PannelData->m_SelectedEntity)
		{
			const auto& entityComponents = registry->EntityComponents(m_PannelData->m_SelectedEntity);

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;
			flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

			for (const auto& comp : entityComponents)
			{

				////ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
				//float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
				//ImVec2 contentRegion = ImGui::GetContentRegionAvail();


				//BaseComponent& component = registry->GetComponent(m_PannelData->m_SelectedEntity, comp);
				//ImGui::Separator();

				//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 0));
				//std::string checkboxLabel = "##Enable Component" + std::to_string(comp);
				//ImGui::Checkbox(checkboxLabel.c_str(), &component.m_Enabled);

				//ImGui::SameLine();

				//bool opened = ImGui::TreeNodeEx(component.Name().data(), flags);

				//ImGui::PopStyleVar();

			
				//ImGui::SameLine(contentRegion.x - lineHeight * .55f);
				//bool removed = false;

				//// Push the style color for the button
				//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
				//ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));

				//if (ImGui::Button("X", ImVec2{ lineHeight, lineHeight }))
				//{
				//	removed = true;
				//}

				//ImGui::PopStyleColor(3);
				//if (opened)
				//{
				//	//std::string checkboxLabel = "##Enable Component" + std::to_string(comp);
				//	ImGui::Checkbox("Serialize in scene", &component.m_SerializeToScene);

				//
				//	ImGui::TreePop();
				//}

				//if (removed)
				//{
				//	eventManager->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(m_PannelData->m_SelectedEntity, comp));
				//}


				factory->ViewComponent(Entity(m_PannelData->m_SelectedEntity, registry), comp);
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
					if (name == "ScriptComponent")
						continue;

					auto id = factory->GetComponentID(name);
					if (!registry->HasComponent(m_PannelData->m_SelectedEntity, id))
					{
						if (ImGui::MenuItem(name.c_str()))
						{
							eventManager->QueueEvent(std::make_unique<EntityAddComponentEvent>(m_PannelData->m_SelectedEntity, id));

							const auto& bin = factory->CreateComponent(name, JSON());
							factory->AddComponentFromBinary(name, bin, registry, m_PannelData->m_SelectedEntity);

							ImGui::CloseCurrentPopup();
						}
					}
				}



				const auto& names = NScriptingInterface::Instance()->GetScriptNames();
				if (ImGui::BeginMenu("Add Script"))
				{

					static ImGuiTextFilter filter;

					filter.Draw("##searchbarBehAdd");

					ImGui::Separator();

					std::vector<std::string> scriptNames;

					auto scripts = registry->GetNamedComponents<ScriptComponent>(m_PannelData->m_SelectedEntity);

					for (ScriptComponent& scriptComponent : scripts)
					{
						scriptNames.push_back(STRFROM(scriptComponent.nameID));
					}


					for (auto name : names)
					{
					
						if (std::find(scriptNames.begin(), scriptNames.end(), name) == scriptNames.end())
							if (filter.PassFilter(name.c_str()))
							{
								if (ImGui::MenuItem(name.c_str()))
								{
									// Handle adding script component here
									eventManager->QueueEvent(std::make_unique<EntityAddComponentEvent>(m_PannelData->m_SelectedEntity, Component<ScriptComponent>::GetID()));

									// For example, create and add script component similar to other components
									const auto& bin = factory->CreateComponent("ScriptComponent", JSON());
									factory->AddNamedComponentFromBinary("ScriptComponent", STRID(name), bin, registry, m_PannelData->m_SelectedEntity);

									eventManager->QueueEvent(std::make_unique<ScriptCreatedEvent>(m_PannelData->m_SelectedEntity, name));
								}
							}
					}


					if (ImGui::MenuItem("New Script"))
					{
						m_ShowCreationMenu = true;
					}
					ImGui::EndMenu();

				}

				ImGui::EndPopup();

			}
		}

		// This opens the modal if the flag was set
		if (m_ShowCreationMenu)
		{
			ImGui::OpenPopup("New Script Name");
		}

		// This is the actual modal window
		if (ImGui::BeginPopupModal("New Script Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Enter the script name:");
			ImGui::InputText("##scriptname", &m_ScriptName);

			if (ImGui::Button("Create", ImVec2(120, 0)))
			{
				m_ShowCreationMenu = false; // Reset flag
				ImGui::CloseCurrentPopup();

				NScriptingInterface::Instance()->CreateScript(m_ScriptName);

				// Handle adding script component here
				eventManager->QueueEvent(std::make_unique<EntityAddComponentEvent>(m_PannelData->m_SelectedEntity, Component<ScriptComponent>::GetID()));

				// For example, create and add script component similar to other components
				const auto& bin = factory->CreateComponent("ScriptComponent", JSON());
				factory->AddNamedComponentFromBinary("ScriptComponent", STRID(m_ScriptName), bin, registry, m_PannelData->m_SelectedEntity);

				eventManager->QueueEvent(std::make_unique<ScriptCreatedEvent>(m_PannelData->m_SelectedEntity, m_ScriptName));

				m_ScriptName = "New Script";
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				m_ShowCreationMenu = false;
				m_ScriptName = "New Script";
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::End();
	}
}
