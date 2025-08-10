#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	EventPropertiesPannel.h
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

	class EventPropertiesPannel : public MontagePannel
	{
	public:
		EventPropertiesPannel() = default;
		~EventPropertiesPannel() = default;

		void OnImGUIRender();
	private:
		EventPropertiesPannel(EventPropertiesPannel const&) = delete;
		EventPropertiesPannel& operator=(EventPropertiesPannel const&) = delete;
	};

}