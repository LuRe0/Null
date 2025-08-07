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
#include "../../../../NullEngine/src/Null/Engine/Submodules/ECS/Components/AnimationComponent.h"
#include <glm/glm.hpp>

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
	//class AnimationClip;
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
        uint32_t selectedSpriteSourceID = 0;
        glm::ivec2 m_GridSize = { 1, 1 };
        glm::ivec2 m_CellSize = { 32, 32 };

        // The actual clip being edited
        AnimationClip workingClip;

        // UI state
        bool isPlaying = false;
        float animLength = 0.50f;
        bool hasUnsavedChanges = false;

        void UpdateClipFromSelection(int startFrame, int endFrame)
        {
            workingClip.startingFrame = startFrame;
            workingClip.frameCount = startFrame >= 0 ? (endFrame - startFrame) + 1 : 0;
            workingClip.frameDuration = animLength / (float)workingClip.frameCount;
            workingClip.spriteSheetID = selectedSpriteSourceID;
            hasUnsavedChanges = true;
        }
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