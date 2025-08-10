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
// File Name:    {{EDITOR_NAME}}.h
// Author(s):    {{ AUTHOR_NAME }}
// Date:         {{ DATE }}
// 
//------------------------------------------------------------------------------



namespace NULLENGINE
{
    class KeyPressEvent;
	class Event;
	class SceneSwitchEvent;

    class {{EDITOR_NAME}} : public ImGuiEditor
    {
    public:
    {{EDITOR_NAME}}();

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float dt) override;
    void OnRender() override;
    void RenderMenuBar() override;
    bool HandleKeyboardShortcuts(const KeyPressEvent & e) override;
    void OnEvent(const Event & e) override;
    void SaveChanges() override
    void Reset() override
    {
        m_PannelData =  {{EDITOR_PANNEL}}Data();
    }
    void AddPannel(std::unique_ptr<{{EDITOR_PANNEL}}>&& pannel);
    void SetPannelData(const MontagePannelData& data);
    void SetPannelParent();

    private:
        std::vector<std::unique_ptr<{{EDITOR_PANNEL}}>> m_Pannels;
        {{EDITOR_PANNEL}}Data m_PannelData;
        void KeyboardShortcuts();
        bool OnKeyPressed(const KeyPressEvent & e);
    };
}
