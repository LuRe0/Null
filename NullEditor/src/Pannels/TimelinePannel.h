#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	TimelinePannel.h
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

	class TimelinePannel : public MontagePannel
	{
	public:
		TimelinePannel() = default;
		~TimelinePannel() = default;

		void OnImGUIRender();
		void RenderMontageTimeline(MontagePannelData& panelData);
	private:
		bool m_ShowCreationMenu = false;
		int m_SelectedStartFrame = -1;
		int m_SelectedEndFrame = -1;
		bool m_IsSelecting = false;
		TimelinePannel(TimelinePannel const&) = delete;
		TimelinePannel& operator=(TimelinePannel const&) = delete;
	};

}