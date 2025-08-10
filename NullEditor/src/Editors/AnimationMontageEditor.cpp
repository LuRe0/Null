
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
#include "AnimationMontageEditor.h"

#include "../Pannels/TimelinePannel.h"
#include "../Pannels/ContentBrowserPannel.h"
#include "../Pannels/MontagePropertiesPannel.h"
#include "../Pannels/MontagePreviewPannel.h"
#include "../Pannels/EventPropertiesPannel.h"
#include "Null/Tools/FileDialog.h"

#include "NIncludes.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    AnimationMontageEditor::AnimationMontageEditor() : ImGuiEditor("Montage Editor", EditorType::ANIMATION_MONTAGE)
    {
        m_IsOpen = false;
        OnAttach();
    }

    void AnimationMontageEditor::OnAttach() 
	{
		auto pannel = std::make_unique<TimelinePannel>();
        auto pannel1 = std::make_unique<ContentBrowserPannel>();
		auto pannel2 = std::make_unique<MontagePropertiesPannel>();
		auto pannel3 = std::make_unique<EventPropertiesPannel>();
		auto pannel4 = std::make_unique<MontagePreviewPannel>();

		AddPannel(std::move(pannel));
		AddPannel(std::move(pannel2));
		AddPannel(std::move(pannel3));
		AddPannel(std::move(pannel4));
		AddGenericPannel(std::move(pannel1));

		SetPannelData(m_PannelData);
		SetPannelParent();


		NEventManager* eventManager = NEventManager::Instance();


		SUBSCRIBE_EVENT(KeyPressEvent, &AnimationMontageEditor::OnKeyPressed, eventManager, EventPriority::Low);
	}
    void AnimationMontageEditor::OnDetach() {}
    void AnimationMontageEditor::OnUpdate(float dt) 
    {
        KeyboardShortcuts();
    }
	void AnimationMontageEditor::SetPannelData(const MontagePannelData& data)
	{
		for (auto& pannel : m_Pannels)
			pannel.get()->SetPannelData(m_PannelData);
	}
	void AnimationMontageEditor::SetPannelParent()
	{
		for (auto& pannel : m_Pannels)
			pannel.get()->SetPannelParent(this);
	}

	void AnimationMontageEditor::OnRender()
	{
        ImGuiEditor::OnRender();

		HandleFileDragDrop();


		for (auto& panel : m_Pannels)
			panel->OnImGUIRender();


        if (m_CODA)
        {
            ImGui::OpenPopup("Unsaved Changes");
        }

        //ImGui::SetNextWindowSize(ImVec2(450, 150), ImGuiCond_Once);

        if (ImGui::BeginPopupModal("Unsaved Changes", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            const char* message = "Are you sure you want to close?";
            ImVec2 textSize = ImGui::CalcTextSize(message);
            ImVec2 windowSize = ImGui::GetContentRegionAvail();
            ImGui::SetCursorPosX((windowSize.x - textSize.x) * 0.5f);
            ImGui::Text("%s", message);

            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            float buttonWidth = 100.0f;
            float buttonSpacing = ImGui::GetStyle().ItemSpacing.x;
            float totalWidth = buttonWidth * 3.0f + buttonSpacing * 2.0f;
            ImGui::SetCursorPosX((windowSize.x - totalWidth) * 0.5f);

            if (ImGui::Button("Save", ImVec2(buttonWidth, 0)))
            {
                SaveCurrentMontageImpl();
                m_CODA = false;
                Reset();
            }
            ImGui::SameLine();

            if (ImGui::Button("Don't Save", ImVec2(buttonWidth, 0)))
            {
                m_CODA = false;
                m_HasUnsavedChanges = false;
                Reset();
                ImGui::CloseCurrentPopup();

            }
            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
            {
                m_CODA = false;
                ImGui::CloseCurrentPopup();

            }

            ImGui::EndPopup();
        }

	}
    void AnimationMontageEditor::RenderMenuBar() 
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Clip", "CTRL+N", false))
            {
                NewMontageImpl();
            }
            if (ImGui::MenuItem("Open Clip", "CTRL+O", false))
            {
                OpenMontageImpl();
            }
            if (ImGui::MenuItem("Save Clip", "CTRL+S", false))
            {
                SaveCurrentMontageImpl();
            }
            ImGui::EndMenu();
        }
    }
    bool AnimationMontageEditor::HandleKeyboardShortcuts(const KeyPressEvent& e) 
    {
        if (NAsyncTaskManager::Instance()->HasPendingTasks())
            return false;

        return true;
    }
    void AnimationMontageEditor::OnEvent(const Event& e) {}
    void AnimationMontageEditor::SaveChanges()
    {
        SaveCurrentMontageImpl();
    }
    void AnimationMontageEditor::KeyboardShortcuts() 
    {
        if (NAsyncTaskManager::Instance()->HasPendingTasks())
            return;

        if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) && ImGui::IsKeyPressed(ImGuiKey_N))
        {
            NewMontageImpl();
        }

        if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) && ImGui::IsKeyPressed(ImGuiKey_O))
        {
            OpenMontageImpl();
        }

        if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) && ImGui::IsKeyPressed(ImGuiKey_S))
        {
            SaveCurrentMontageImpl();
        }
    }
    bool AnimationMontageEditor::OnKeyPressed(const KeyPressEvent& e)
    {
        return HandleKeyboardShortcuts(e);
    }

    void AnimationMontageEditor::NewMontageImpl()
    {
        m_CODA = HasUnsavedChanges();
    }

    void AnimationMontageEditor::SaveCurrentMontageImpl()
    {
        if (HasUnsavedChanges())
        {
            NAnimationClipManager::Instance()->SaveClipToFile(*m_PannelData.selectedClip);
            m_HasUnsavedChanges = false;
            NLE_CORE_INFO("Changes saved in Animation Montage Editor");
		}
    }

    void AnimationMontageEditor::OpenMontageImpl()
    {

        const std::string& stem = FileDialog::OpenFile("Null Engine Scene (*.anim)\0*.anim\0");

        if (!stem.empty())
        {
            std::string fullPath = "../Assets/Animations/" + stem + ".anim";
            LoadAnimationClipFile(fullPath);
        }


    }

    void AnimationMontageEditor::AddPannel(std::unique_ptr<MontagePannel>&& pannel)
	{
		m_Pannels.push_back(std::move(pannel));
	}


    void AnimationMontageEditor::HandleFileDragDrop()
    {
        if (HasUnsavedChanges())
        {
			NLE_CORE_WARN("Unable to load new animation clip while there are unsaved changes.");
			return;
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ANIM_MONTAGE_FILE"))
            {
                std::string stem((const char*)payload->Data);
                std::string fullPath = "../Assets/Animations/" + stem + ".anim";
                LoadAnimationClipFile(fullPath);
				m_HasUnsavedChanges = false; // Reset unsaved changes flag after loading
				//Reset();
            }

            ImGui::EndDragDropTarget();
        }
    }

    void AnimationMontageEditor::LoadAnimationClipFile(const std::string& filepath)
    {
        // Extract filename without extension for the clip name
        std::filesystem::path path(filepath);
        std::string clipName = path.stem().string();

        // Let the manager handle the loading
        NAnimationClipManager* clipManager = NAnimationClipManager::Instance();


        // Create or get existing clip and load from JSON
        AnimationClip* clip = clipManager->Get(clipName);
        if (!clip)
        {


            // Read and parse the JSON manually (since manager's Load() only does folder scanning)
            std::ifstream file(filepath);
            if (!file.is_open()) {
                NLE_CORE_ERROR("Failed to open animation file: {0}", filepath);
                return;
            }

            nlohmann::json json;
            file >> json;
            file.close();

            clip = clipManager->CreateRaw(clipName);

            clipManager->LoadClipFromJson(*clip, json);

        }

        // Now load it into your editor
        //LoadAnimationClip(*clip);
        m_PannelData.selectedClip = clip;
		m_PannelData.selectedSpriteSource = NSpriteSourceManager::Instance()->Get(clip->spriteSheetID);

        NLE_CORE_INFO("Animation Clip: {0} loaded from {1}", clipName, filepath);
    }

}
