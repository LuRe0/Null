#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NStub.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "ImGuiEditor.h"
#include "Null/Engine/Submodules/Layers/Layer.h"
#include <glm/glm.hpp>
#include <memory>
#include "../Pannels/Base/Pannel.h"

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
    class Scene;
    class Camera;
    class OrthographicCameraController;
    class PerspectiveCameraController;
    class CameraController;
    class AnimationClipEditor;
    class AnimationPannelData;
    //class Pannel;
    class KeyPressEvent;
    class Event;
    class SceneSwitchEvent;


    class NLE_API AnimationClipEditor : public ImGuiEditor
    {
    public:
        AnimationClipEditor();

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float dt) override;
        void OnRender() override;
        void RenderMenuBar() override; // Renders the menu bar
        bool HandleKeyboardShortcuts(const KeyPressEvent& e) override;
        void OnEvent(const Event& e) override;
        void AddPannel(std::unique_ptr<AnimationPannel>&& pannel);

        void SetPannelData(const AnimationPannelData& data);
        void SetPannelParent();

        // Scene-specific methods
    private:
        std::vector<std::unique_ptr<AnimationPannel>> m_Pannels;
        AnimationPannelData m_PannelData;

        //AnimationClip m_CurrentClip;
        //std::vector<AnimationClip> m_ClipLibrary;



        //PannelData m_PannelData;
        void KeyboardShortcuts();
        bool OnKeyPressed(const KeyPressEvent& e);

        void ExportClipsImpl();
        void SaveCurrentClipImpl();
        void OpenClipLibraryImpl();
        void NewClipImpl();


        //bool OnSceneSwitched(const SceneSwitchEvent& e);
    };


};