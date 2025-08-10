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
	class AnimationClip;
	class ImGuiEditor;
	class AnimationClipEditor;
	class SpriteSource;
	class AnimationMontageEditor;
	class Scene;
	using EntityID = uint32_t;

	struct MontagePannelData
    {
		AnimationClip* selectedClip = nullptr;
		SpriteSource* selectedSpriteSource = nullptr;
		float m_PlaybackTime = 0.0f;
        bool isPlaying = false;
		int currentFrame = 0;
    };

    struct ScenePannelData
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

        void UpdateClipFromSelection(int startFrame, int endFrame)
        {
            workingClip.startingFrame = startFrame;
            workingClip.frameCount = startFrame >= 0 ? (endFrame - startFrame) + 1 : 0;
            //workingClip.animationLength = animLength / (float)workingClip.frameCount;
            workingClip.spriteSheetID = selectedSpriteSourceID;
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
		void SetID(uint32_t id) { m_ID = id; }
        virtual void OnImGUIRender() = 0;

    protected:
        DataType* m_PannelData = nullptr;
        ParentType* m_Parent = nullptr;

        uint32_t m_ID;
    };

    // Usage:
    using ScenePannel = Pannel<ScenePannelData, SceneEditor>;
    using AnimationPannel = Pannel<AnimationPannelData, AnimationClipEditor>;
    using MontagePannel = Pannel<MontagePannelData, AnimationMontageEditor>;
    struct EmptyData {};
    using GenericPannel = Pannel<EmptyData, ImGuiEditor>;


}