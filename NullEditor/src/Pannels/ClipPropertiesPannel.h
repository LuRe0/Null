#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	ClipPropertiesPannel.h
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
	struct AnimationFlagSet;

	class ClipPropertiesPannel : public AnimationPannel
	{
	public:
		ClipPropertiesPannel() = default;
		~ClipPropertiesPannel() = default;

		void OnImGUIRender();
	private:
		ClipPropertiesPannel(ClipPropertiesPannel const&) = delete;
		ClipPropertiesPannel& operator=(ClipPropertiesPannel const&) = delete;

		char m_ClipNameBuffer[256] = "NewClip";
		float m_FrameDuration = 0.1f;
		bool m_IsLooping = true;
		bool m_PreserveFrame = true;
		bool m_PingPong = true;
		bool m_Reversed = true;

	};

}