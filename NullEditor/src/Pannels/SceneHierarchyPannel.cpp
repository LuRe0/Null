
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
//#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include "../Editors/SceneEditor.h"
#include "NIncludes.h"
#include "../../../NullEngine/src/Null/Engine/Submodules/ECS/Entities/Entity.h"

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


		ImGui::PushItemWidth(200);
		ImGui::Text("Scene Name: "); ImGui::SameLine();

		if (ImGui::InputText("##name", &m_PannelData->m_Context->m_Name))
		{
			if (m_PannelData->m_Context->m_Name.empty())
			{
				m_PannelData->m_Context->m_Name = "New Scene";
			}
		}

		if (m_PannelData->m_Context->m_Name == "New Scene")
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Must Change Scene Name to Save");

		ImGui::PopItemWidth();





		ImGui::BeginChild("##target");

		auto& entities = m_PannelData->m_Context->ManagedEntities();

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_PannelData->m_SelectedEntity = {};

		// Create component
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Create Entity"))
				m_PannelData->m_SelectedEntity = m_PannelData->m_Context->CreateEmptyEntity(m_PannelData->m_Context->GenerateUniqueName("New Entiy"));


			if (ImGui::MenuItem("Load Archetype"))
			{
				const std::string& archetype = FileDialog::OpenFile("Null Engine archetype (*.ent)\0*.ent\0");
				if (!archetype.empty())
				{
					m_PannelData->m_SelectedEntity = {};
					m_PannelData->m_Context->LoadArchetype(archetype);
				}
			}
			ImGui::EndPopup();
		}


		static ImGuiTextFilter m_EntityFilter;

		m_EntityFilter.Draw("Search Entity");

		ImGui::Separator();

		for (auto& entity : entities)
		{

			if (entity.Has<ParentComponent>())
				continue;

			
			const std::string name = STRFROM(entity.Get<NameComponent>().nameID); // or however you get the entity's name

			if (!m_EntityFilter.PassFilter(name.c_str()))
				continue;

			ImGuiTreeNodeFlags flags = (m_PannelData->m_SelectedEntity == entity.GetID() ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

			if (entity.Has<ChildrenComponent>()) 
			{
				flags |= ImGuiTreeNodeFlags_DefaultOpen;
			}


			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 100);
			std::string id = "ID:" + std::to_string(entity.GetID());
			ImGui::TextColored(ImVec4(0, 1, 0, 1), id.c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosX(0);
			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)(entity.GetID()), flags, name.c_str());

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				if (m_PannelData->m_SelectedEntity == entity.m_ID)
				{

					ImGui::SetDragDropPayload("ENTITY_TARGET", &entity.m_ID, sizeof(uint32_t));
					ImGui::Text("Dragging %s", name.c_str());
				}
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
	

					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TARGET"))
					{
						uint32_t entityID = *((uint32_t*)payload->Data);

						ParentEntities(m_PannelData->m_Context->GetEntity(entityID), entity);
					}

				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_CHILD_TARGET"))
				{
					uint32_t entityID = *((uint32_t*)payload->Data);

					SeparateEntities(m_PannelData->m_Context->GetEntity(entityID));

					ParentEntities(m_PannelData->m_Context->GetEntity(entityID), entity);

				}

				ImGui::EndDragDropTarget();
			}


			if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
			{
				m_PannelData->m_SelectedEntity = entity.GetID();
			}

			SelectedEntityMenuItems(entity);

			if (opened)
			{
				if (entity.Has<ChildrenComponent>())
				{
					auto& cComp = entity.Get<ChildrenComponent>();
					auto& children = cComp.m_Children;


					for (const auto& childID : children)
					{
						auto& childEntity = m_PannelData->m_Context->GetEntity(childID);

						auto& nComp = childEntity.Get<NameComponent>();
						const std::string childName = STRFROM(nComp.nameID);

						// Recursive call to draw the child entity and its children
						DrawEntityNode(childEntity);

						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
						{
							if (m_PannelData->m_SelectedEntity == childEntity)
							{
			
								ImGui::SetDragDropPayload("ENTITY_CHILD_TARGET", &childEntity.m_ID, sizeof(uint32_t));
								ImGui::Text("Dragging %s", childName.c_str());
							}
							ImGui::EndDragDropSource();
						}

					

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


		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_CHILD_TARGET"))
			{
				uint32_t entityID = *((uint32_t*)payload->Data);

				SeparateEntities(m_PannelData->m_Context->GetEntity(entityID));
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::End();
	}
	void SceneHierarchyPannel::DrawEntityNode(Entity& entity)
	{
		ImGuiTreeNodeFlags flags = (m_PannelData->m_SelectedEntity == entity.GetID() ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

		if (entity.Has<ChildrenComponent>())
		{
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

		auto& nComp = entity.Get<NameComponent>();

		const std::string name = STRFROM(nComp.nameID); // 

		ImVec2 cursorPos = ImGui::GetCursorPos(); // Save the current cursor position

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 100);
		std::string id = "ID:" + std::to_string(entity.GetID());
		ImGui::TextColored(ImVec4(0, 1, 0, 1), id.c_str());
		ImGui::SameLine();

		ImGui::SetCursorPos(cursorPos);
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)(entity.GetID()), flags, name.c_str());

		if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
		{
			m_PannelData->m_SelectedEntity = entity.GetID();
		}

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			if (m_PannelData->m_SelectedEntity == entity)
			{
				ImGui::SetDragDropPayload("ENTITY_CHILD_TARGET", &entity.m_ID, sizeof(uint32_t));
				ImGui::Text("Dragging %s", name.c_str());
			}
			ImGui::EndDragDropSource();
		}


		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_CHILD_TARGET"))
			{
				uint32_t entityID = *((uint32_t*)payload->Data);

				SeparateEntities(m_PannelData->m_Context->GetEntity(entityID));

				ParentEntities(m_PannelData->m_Context->GetEntity(entityID), entity);

			}

			ImGui::EndDragDropTarget();
		}


		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TARGET"))
			{
				uint32_t entityID = *((uint32_t*)payload->Data);

				ParentEntities(m_PannelData->m_Context->GetEntity(entityID), entity);
			}

			ImGui::EndDragDropTarget();
		}


		SelectedEntityMenuItems(entity);

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
	void SceneHierarchyPannel::ParentEntities(Entity& entity, Entity& parentEntity)
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		if(!parentEntity.Has<ChildrenComponent>())
			parentEntity.Add<ChildrenComponent>();

		auto& cComp = parentEntity.Get<ChildrenComponent>();
		cComp.m_Children.push_back(entity.m_ID);


		if (!entity.Has<ParentComponent>())
			entity.Add< ParentComponent>();

		if (!entity.Has<ArchetypeComponent>())
			entity.Add<ArchetypeComponent>();

		auto& archetypeComp = entity.Get<ArchetypeComponent>();
		archetypeComp.parentArchetypeID = parentEntity.Get<ArchetypeComponent>().archetypeID;


		auto& pComp = entity.Get<ParentComponent>();
		pComp.m_Parent = parentEntity.m_ID;

		// TO DO: ENTITY PARENTED ENVENT

		eventManager->QueueEvent(std::make_unique<EntityParentedEvent>(parentEntity.m_ID, entity.m_ID));

	}
	void SceneHierarchyPannel::SeparateEntities(Entity& entity)
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		if (entity.Has<ParentComponent>())
		{
			if(entity.Has<ArchetypeComponent>())
				entity.Get<ArchetypeComponent>().parentArchetypeID = 0; // Reset archetype ID if needed

			auto& pComp = entity.Get<ParentComponent>();

			Entity& parentEntity = m_PannelData->m_Context->GetEntity(pComp.m_Parent);

			eventManager->QueueEvent(std::make_unique<EntitySeparatedEvent>(parentEntity.m_ID, entity.m_ID));


			auto& cComp = parentEntity.Get<ChildrenComponent>();
			auto& vec = cComp.m_Children;
			vec.erase(std::remove(vec.begin(), vec.end(), entity.m_ID), vec.end());
			if (vec.empty())
			{
				parentEntity.Remove < ChildrenComponent >();
			}
			entity.Remove<ParentComponent>();
		}


		// TO DO: ENTITY UNPARENTED ENVENT
	}
	void SceneHierarchyPannel::SelectedEntityMenuItems(Entity& entity)
	{
		if (ImGui::BeginPopupContextItem())
		{

			if (ImGui::MenuItem("Create Entity (Child)"))
			{
				m_PannelData->m_SelectedEntity = m_PannelData->m_Context->CreateEmptyEntity("New Child Entiy");

				ParentEntities(m_PannelData->m_Context->GetEntity(m_PannelData->m_SelectedEntity), entity);
			}


			if (ImGui::MenuItem("Save as new Archetype"))
			{
				const uint32_t& archetype = STRID(FileDialog::SaveFile("Null Engine Archetype (*.ent)\0*.ent\0"));
				if (archetype)
				{
					m_PannelData->m_Context->SerializeArchetype(archetype, m_PannelData->m_SelectedEntity);
				}
			}

			if (ImGui::MenuItem("Save Archetype"))
			{
				Entity& entity = m_PannelData->m_Context->GetEntity(m_PannelData->m_SelectedEntity);
				if (entity.Has<ArchetypeComponent>())
				{
					ArchetypeComponent& archetypeComp = entity.Get<ArchetypeComponent>();
					m_PannelData->m_Context->SerializeArchetype(archetypeComp.archetypeID, m_PannelData->m_SelectedEntity);
				}
			}


			if (ImGui::MenuItem("Delete Entity"))
			{
				m_PannelData->m_Context->DeleteEntity(entity.GetID());

				if (entity.GetID() == m_PannelData->m_SelectedEntity)
				{
					m_PannelData->m_SelectedEntity = {};
				}
			}

			ImGui::EndPopup();

		}

	}
}
