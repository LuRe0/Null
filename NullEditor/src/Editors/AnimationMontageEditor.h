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


#pragma once

//------------------------------------------------------------------------------
//
// File Name:    AnimationMontageEditor.h
// Author(s):    {{ AUTHOR_NAME }}
// Date:         {{ DATE }}
// 
//------------------------------------------------------------------------------



namespace NULLENGINE
{
    class KeyPressEvent;
	class Event;
	class SceneSwitchEvent;

    class AnimationMontageEditor : public ImGuiEditor
    {
    public:
    AnimationMontageEditor();

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float dt) override;
    void SetPannelData(const MontagePannelData& data);
    void SetPannelParent();
    void OnRender() override;
    void RenderMenuBar() override;
    bool HandleKeyboardShortcuts(const KeyPressEvent & e) override;
    void OnEvent(const Event & e) override;
    void SaveChanges() override;

    void Reset() override
    {
        m_PannelData = MontagePannelData();
    }
    void AddPannel(std::unique_ptr<MontagePannel>&& pannel);

        void HandleFileDragDrop();
    private:
        void LoadAnimationClipFile(const std::string& filepath);
        //void ShowFileLoadDialog();
        //void LoadAnimationClip(const AnimationClip& clip);

        std::vector<std::unique_ptr<MontagePannel>> m_Pannels;
        MontagePannelData m_PannelData;
        void KeyboardShortcuts();
        bool OnKeyPressed(const KeyPressEvent & e);

        void NewMontageImpl();
		void SaveCurrentMontageImpl();
        void OpenMontageImpl();
    };
}
