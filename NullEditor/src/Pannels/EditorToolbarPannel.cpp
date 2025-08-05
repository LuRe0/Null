
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
#include "EditorToolbarPannel.h"
#include "Null/Engine/Submodules/Scene.h"
//#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>
#include "Null/Tools/ImguiSink.h"
//#include "backends/imgui_impl_opengl3.h"
//#include "backends/imgui_impl_glfw.h"
#include "../Editors/SceneEditor.h"
#include "NIncludes.h"

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	//void EditorToolbarPannel::SetContext(Scene* scene)
	//{
	//	m_Context = scene;
	//}


	EditorToolbarPannel::EditorToolbarPannel()
	{
		m_PlayButton = Texture(std::string("../Assets/Resources/Textures/PlayButton.png"), true);
		m_StopButton = Texture(std::string("../Assets/Resources/Textures/StopButton.png"), true);

		m_PlayButton.Init();
		m_StopButton.Init();
	}

    void EditorToolbarPannel::OnImGUIRender()
    {
        NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();
        NDebugManager* debugManager = NEngine::Instance().Get<NDebugManager>();
        NFramebufferManager* fbMan = NEngine::Instance().Get<NFramebufferManager>();

        // Cleaner window setup
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));

        if (ImGui::Begin("##UI_Toolbar", nullptr, windowFlags))
        {
            float availableWidth = ImGui::GetContentRegionAvail().x;
            float buttonSize = ImGui::GetFrameHeight();

            // Left section: Play/Stop button
            RenderPlayControls(buttonSize);

            ImGui::SameLine();

            // Middle section: Debug controls
            RenderDebugControls(debugManager);

            // Right section: Render settings (right-aligned)
            float rightSectionWidth = 450.0f; // Approximate width needed
            ImGui::SameLine(availableWidth - rightSectionWidth);

            RenderRenderSettings(fbMan);
        }
        ImGui::End();

        ImGui::PopStyleVar(2);
    }

    void EditorToolbarPannel::RenderPlayControls(float buttonSize)
    {
        NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

        // Play/Stop button
        bool isPlaying = NEngine::Instance().GetEngineState() != IEngine::EDIT;
        Texture& icon = isPlaying ?
            *NTextureManager::Instance()->Get("StopButton") :
            *NTextureManager::Instance()->Get("PlayButton");

        if (ImGui::ImageButton("##PlayStop", (ImTextureID)icon.GetID(), ImVec2(buttonSize, buttonSize)))
        {
            if (isPlaying)
            {
                // Stop
                NEngine::Instance().SetEngineState(IEngine::EDIT);
                eventManager->QueueEvent(std::make_unique<SceneSwitchEvent>(
                    m_PannelData->m_Context->GetName(),
                    m_PannelData->m_Context->GetName()));
                eventManager->QueueEvent(std::make_unique<EngineEditStateEvent>(NEngine::EDIT));
            }
            else
            {
                // Play
                m_Parent->SaveSceneImpl();

                if (m_CurrentMode == MODE_WINDOWED)
                {
                    NEngine::Instance().SetEngineState(IEngine::RUN_WINDOWED);
                    eventManager->QueueEvent(std::make_unique<EngineRunStateEvent>(NEngine::RUN_WINDOWED));
                    m_Parent->ResetViewportSize();
                }
                else
                {
                    NEngine::Instance().SetEngineState(IEngine::RUN_MAXIMIZED);
                    eventManager->QueueEvent(std::make_unique<EngineRunStateEvent>(NEngine::RUN_MAXIMIZED));
                }
            }
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(isPlaying ? "Stop" : "Play");
        }
    }

    void EditorToolbarPannel::RenderDebugControls(NDebugManager* debugManager)
    {
        // Debug checkbox
        ImGui::Text("Debug:");
        ImGui::SameLine();
        ImGui::Checkbox("##ShowDebug", &debugManager->m_ShowDebug);

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Show debug overlays and information");
        }
    }

    void EditorToolbarPannel::RenderRenderSettings(NFramebufferManager* fbMan)
    {
        // Window mode dropdown
        ImGui::Text("Mode:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        if (ImGui::BeginCombo("##WindowMode", m_CurrentMode == MODE_MAXIMIZED ? "Maximized" : "Windowed"))
        {
            if (ImGui::Selectable("Maximized", m_CurrentMode == MODE_MAXIMIZED))
                m_CurrentMode = MODE_MAXIMIZED;

            if (ImGui::Selectable("Windowed", m_CurrentMode == MODE_WINDOWED))
                m_CurrentMode = MODE_WINDOWED;

            ImGui::EndCombo();
        }

        ImGui::SameLine();

        // Render target dropdown
        ImGui::Text("Target:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        if (ImGui::BeginCombo("##RenderTarget", m_Parent->GetRenderTarget().c_str()))
        {
            const auto& fbNames = fbMan->GetResourceNames();
            for (const auto& name : fbNames)
            {
                bool isSelected = (m_Parent->GetRenderTarget() == name);
                if (ImGui::Selectable(name.c_str(), isSelected))
                    m_Parent->SetRenderTarget(name);

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
}
