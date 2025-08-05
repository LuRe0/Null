#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	ImGuiEditor.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include <string>
#include "../Pannels/Base/Pannel.h"

//#include "stdafx.h"

//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//


namespace NULLENGINE
{
	class KeyPressEvent;
	class Event;
    class ImGuiLayer;

    enum class EditorType
    {
        SCENE,
        ANIMATION_CLIP,
        STATE_MACHINE,
        ARCHETYPE,
        BEHAVIOR_TREE
    };

	class NLE_API ImGuiEditor
	{
	public:
        ImGuiEditor(const std::string& name, EditorType type)
            : m_Name(name), m_Type(type), m_IsOpen(false) {
        }
        virtual ~ImGuiEditor() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(float dt) {}
        virtual void OnRender() = 0; // Renders the docked window
        virtual void RenderMenuBar() = 0; // Renders the docked window
        virtual bool HandleKeyboardShortcuts(const KeyPressEvent& e) { return false; }
        virtual void OnEvent(const Event& e) {}
        void SetParent(ImGuiLayer* p) { m_Parent = p; }

        void Show() { m_IsOpen = true; }
        void Hide() { m_IsOpen = false; }
        void Toggle() { m_IsOpen = !m_IsOpen; }
        bool IsOpen() const { return m_IsOpen; }
        void SetShouldSelect(bool should) { m_ShouldSelect = should; }
        bool ShouldSelect() const { return m_ShouldSelect; }
        const std::string& GetName() const { return m_Name; }
        EditorType GetType() const { return m_Type; }

    protected:
        std::string m_Name;
        EditorType m_Type;
        bool m_IsOpen;
        bool m_CODA = false;
        bool m_HasUnsavedChanges = false;
        ImGuiLayer* m_Parent;

        // Helper to create the editor's main window with dockspace
        bool BeginEditorWindow(ImGuiWindowFlags extra_flags = 0);
        void EndEditorWindow();

        // Helper for panels within the editor
        bool BeginDockableWindow(const char* name, bool* p_open = nullptr, int flags = 0);
        void EndDockableWindow();

    private:
        bool m_ShouldSelect = false;

	};

}