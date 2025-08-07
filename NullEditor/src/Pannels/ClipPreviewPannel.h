#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	ClipPreviewPannel.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Base/Pannel.h"

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
	class SpriteSource;

	class ClipPreviewPannel : public AnimationPannel
	{
	public:
		ClipPreviewPannel() = default;
		~ClipPreviewPannel() = default;

		void OnImGUIRender();
		void RenderAnimationClipPreview(AnimationPannelData& animData, SpriteSource* spriteSource);
		int GetCurrentAnimationFrame(const AnimationClip& clip, float playbackTime);
		float GetAnimationProgress(const AnimationClip& clip, float playbackTime);
		float GetTotalAnimationDuration(const AnimationClip& clip);
	private:
		ClipPreviewPannel(ClipPreviewPannel const&) = delete;
		ClipPreviewPannel& operator=(ClipPreviewPannel const&) = delete;

		float m_PlaybackTime = 0;
	};

}