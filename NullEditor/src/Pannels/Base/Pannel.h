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
	class SceneEditor;
	class AnimationClip;
	class AnimationClipEditor;
	class Scene;
	using EntityID = uint32_t;

  

    struct PannelData
    {
        EntityID m_SelectedEntity = 0;
        Scene* m_Context = nullptr;
    };

    struct AnimationPannelData
    {
        // Single clip workflow
        AnimationClip* m_CurrentClip = nullptr;

        // Sprite sheet data  
        uint32_t* m_SelectedSpriteSourceID = nullptr;
        glm::ivec2* m_GridSize = nullptr;
        glm::ivec2* m_FrameSelectionStart = nullptr;
        glm::ivec2* m_FrameSelectionEnd = nullptr;

        // UI state
        bool* m_IsPlaying = nullptr;
        float* m_PlaybackTime = nullptr;
        bool* m_HasUnsavedChanges = nullptr;
    };

    template<typename DataType, typename ParentType>
    class NLE_API Pannel
    {
    public:
        Pannel() = default;
        virtual ~Pannel() = default;

        void SetPannelData(DataType& data) { m_PannelData = &data; }
        void SetPannelParent(ParentType* parent) { m_Parent = parent; }
        virtual void OnImGUIRender() = 0;

    protected:
        DataType* m_PannelData = nullptr;
        ParentType* m_Parent = nullptr;
    };

    // Usage:
    using ScenePannel = Pannel<PannelData, SceneEditor>;
    using AnimationPannel = Pannel<AnimationPannelData, AnimationClipEditor>;


}