#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	MontagePreviewPannel.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
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
	class AnimationClip;

	class MontagePreviewPannel : public MontagePannel
	{
	public:
		MontagePreviewPannel() = default;
		~MontagePreviewPannel() = default;

		void OnImGUIRender();
		void RenderAnimationClip(SpriteSource* spriteSource);
		int GetCurrentAnimationFrame(const AnimationClip& clip, float playbackTime);
	private:
		float m_PlaybackTime = 0.0f;
		MontagePreviewPannel(MontagePreviewPannel const&) = delete;
		MontagePreviewPannel& operator=(MontagePreviewPannel const&) = delete;
	};

}