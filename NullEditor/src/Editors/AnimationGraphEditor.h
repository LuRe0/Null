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
#include "../Pannels/Base/Pannel.h"
#include <memory>

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
	//class AnimationGraphEditor;
	class PannelData;
	//class Pannel;
	class KeyPressEvent;
	class Event;
	class SceneSwitchEvent;


    class NLE_API AnimationGraphEditor : public ImGuiEditor
    {
    public:
        AnimationGraphEditor();

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float dt) override;
        void OnRender() override;
        bool HandleKeyboardShortcuts(const KeyPressEvent& e) override;
        void OnEvent(const Event& e) override;

        void AddPannel(std::unique_ptr<AnimationPannel>&& pannel);

    private:
        // Your existing scene-specific members
        std::vector<std::unique_ptr<AnimationPannel>> m_Pannels;
        PannelData m_PannelData;
        glm::vec2 m_ViewportSize = { 0, 0 };
        glm::vec2 m_viewportBounds[2];
  
        bool m_CODA = false;

        // Scene-specific rendering methods

        bool OnKeyPressed(const KeyPressEvent& e);
        bool OnSceneSwitched(const SceneSwitchEvent& e);
    };


}