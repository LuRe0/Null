
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
#include "ImGuiEditor.h"
#include "imgui.h"
#include "NIncludes.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    void ImGuiEditor::AddGenericPannel(std::unique_ptr<GenericPannel>&& pannel)
    {
        m_GenericPannels.push_back(std::move(pannel));
    }
    void ImGuiEditor::OnRender()
    {
        for (auto& pannel : m_GenericPannels)
        {
            pannel->OnImGUIRender();
        }
	}
    bool ImGuiEditor::BeginEditorWindow(ImGuiWindowFlags extra_flags)
    {
        // Create fullscreen invisible window for this editor
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        // Make it fill the main viewport (like the original code)
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        // Remove window decorations
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        bool isOpen = m_IsOpen;
        if (ImGui::Begin((m_Name + "##DockSpace").c_str(), &isOpen, window_flags))
        {
            ImGui::PopStyleVar(3);

            m_IsOpen = isOpen;

            // Create dockspace
            ImGuiID dockspace_id = ImGui::GetID((m_Name + "_DockSpace").c_str());
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

            return true;
        }

        ImGui::PopStyleVar(3);
        m_IsOpen = isOpen;
        return false;
    }


    void ImGuiEditor::EndEditorWindow()
    {
        ImGui::End();
    }

    bool ImGuiEditor::BeginDockableWindow(const char* name, bool* p_open, int flags)
    {
        return ImGui::Begin(name, p_open, flags);
    }

    void ImGuiEditor::EndDockableWindow()
    {
        ImGui::End();
    }
}
