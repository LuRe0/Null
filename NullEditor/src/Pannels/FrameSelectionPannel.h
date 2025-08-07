#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	FrameSelectionPannel.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Base/Pannel.h"
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
	class Scene;
	class SpriteSource;

	class FrameSelectionPannel : public AnimationPannel
	{
	public:
		FrameSelectionPannel() = default;
		~FrameSelectionPannel() = default;

		void OnImGUIRender();
		void DrawSpriteGrid(SpriteSource* spriteSource);
	private:
		void HandleSelection();
		FrameSelectionPannel(FrameSelectionPannel const&) = delete;
		FrameSelectionPannel& operator=(FrameSelectionPannel const&) = delete;

		glm::ivec2 m_GridSize = { 1, 1 };        
		glm::ivec2 m_CellSize = { 32, 32 };      
		int m_SelectedStartFrame = -1;
		int m_SelectedEndFrame = -1;
		float m_MaxDisplaySize = 512.0f;
		bool m_IsSelecting = false;
	};

}