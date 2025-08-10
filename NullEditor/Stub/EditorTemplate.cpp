
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
#include "{{EDITOR_NAME}}.h"





//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    {{EDITOR_NAME}}::{{EDITOR_NAME}}() : ImGuiEditor("{{EDITOR_NAME}}", EditorType::CUSTOM)
    {
        m_IsOpen = false;
        OnAttach();
    }

    void {{EDITOR_NAME}}::OnAttach() {}
    void {{EDITOR_NAME}}::OnDetach() {}
    void {{EDITOR_NAME}}::OnUpdate(float dt) {}
    void {{EDITOR_NAME}}::OnRender() {}
    void {{EDITOR_NAME}}::RenderMenuBar() {}
    bool {{EDITOR_NAME}}::HandleKeyboardShortcuts(const KeyPressEvent& e) { return true; }
    void {{EDITOR_NAME}}::OnEvent(const Event& e) {}
    void {{EDITOR_NAME}}::KeyboardShortcuts() {}
    bool {{EDITOR_NAME}}::OnKeyPressed(const KeyPressEvent& e) { return true; }


	void  {{EDITOR_NAME}}::SaveChanges()
	{
		
	}
    void {{EDITOR_NAME}}::AddPannel(std::unique_ptr<{{EDITOR_PANNEL}}>&& pannel)
	{
		m_Pannels.push_back(std::move(pannel));
	}

    void {{PANNEL_NAME}}::SetPannelData(const MontagePannelData& data)
	{
		for (auto& pannel : m_Pannels)
			pannel.get()->SetPannelData(m_PannelData);
	}
	void {{PANNEL_NAME}}::SetPannelParent()
	{
		for (auto& pannel : m_Pannels)
			pannel.get()->SetPannelParent(this);
	}

	void {{PANNEL_NAME}}::OnRender()
	{
		ImGuiEditor::OnRender();
		
		for (auto& panel : m_Pannels)
			panel->OnImGUIRender();
	}

}
