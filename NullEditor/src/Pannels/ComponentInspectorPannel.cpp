
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

					bool opened = ImGui::TreeNodeEx(component.Name().data(), flags);
					float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
					ImVec2 contentRegion = ImGui::GetContentRegionAvail();
					ImGui::SameLine(contentRegion.x + lineHeight*.75f);
					bool removed = false;
					if (ImGui::Button("X", ImVec2{ lineHeight, lineHeight }))
					{
						removed = true;
					}

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

			if (ImGui::Button("Add Component"))
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
