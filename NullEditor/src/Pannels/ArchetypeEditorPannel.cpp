
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
#include "ArchetypeEditorPannel.h"

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
	void NULLENGINE::ArchetypeEditorPannel::OpenArchetype(const std::string& archetypeName)
	{
		// Create minimal scene
		m_ArchetypeScene = std::make_unique<Scene>("ArchetypeEdit_" + archetypeName);
		m_ArchetypeScene->Init();

		// Load archetype
		m_ArchetypeEntity = m_ArchetypeScene->LoadArchetype(archetypeName);
		m_CurrentArchetypeName = archetypeName;
		m_IsEditing = true;

		// Update panel data to point to archetype scene
		m_PannelData->m_Context = m_ArchetypeScene.get();
		m_PannelData->m_SelectedEntity = m_ArchetypeEntity;
	}

	void ArchetypeEditorPannel::OnImGUIRender()
    {
        if (!m_IsEditing) {
            ImGui::Begin("Archetype Editor");
            ImGui::Text("No archetype loaded");
            if (ImGui::Button("Open Archetype")) {
                // File dialog to open archetype
                const std::string& archetype = FileDialog::OpenFile("Null Engine archetype (*.ent)\0*.ent\0");
                if (!archetype.empty()) {
                    OpenArchetype(archetype);
                }
            }
            ImGui::End();
            return;
        }

        ImGui::Begin("Archetype Editor");

        // Show current archetype info
        ImGui::Text("Editing: %s", m_CurrentArchetypeName.c_str());
        ImGui::Separator();

        // Save button
        if (ImGui::Button("Save Archetype") || (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S))) {
            SaveArchetype();
        }
        ImGui::SameLine();
        if (ImGui::Button("Close")) {
            CloseArchetype();
        }

        ImGui::Separator();

        // Show entity hierarchy (simplified version of your scene hierarchy)
        DrawArchetypeHierarchy();

        ImGui::End();
    }

    void ArchetypeEditorPannel::DrawArchetypeHierarchy()
    {
        if (!m_ArchetypeScene || m_ArchetypeEntity == 0) return;

        Entity& entity = m_ArchetypeScene->GetEntity(m_ArchetypeEntity);

        // Simplified hierarchy drawing - just this entity and its children
        //DrawEntityNode(entity);
    }


    void ArchetypeEditorPannel::SaveArchetype()
    {
        if (m_ArchetypeScene && m_ArchetypeEntity != 0) {
            // Use your existing archetype serialization
            uint32_t archetypeID = STRID(m_CurrentArchetypeName);
            m_ArchetypeScene->SerializeArchetype(archetypeID, m_ArchetypeEntity);
            NLE_CORE_INFO("Archetype '{}' saved", m_CurrentArchetypeName);
        }
    }

    void ArchetypeEditorPannel::CloseArchetype()
    {
        m_IsEditing = false;
        m_ArchetypeScene.reset();
        m_ArchetypeEntity = 0;

        // Restore to main scene
        m_PannelData->m_Context = NSceneManager::Instance()->GetCurrentScene();
        m_PannelData->m_SelectedEntity = {};
    }


}
