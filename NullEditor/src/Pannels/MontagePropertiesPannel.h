#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	MontagePropertiesPannel.h
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

	class MontagePropertiesPannel : public MontagePannel
	{
	public:
		MontagePropertiesPannel() = default;
		~MontagePropertiesPannel() = default;

		void OnImGUIRender();
	private:
		MontagePropertiesPannel(MontagePropertiesPannel const&) = delete;
		MontagePropertiesPannel& operator=(MontagePropertiesPannel const&) = delete;
	};

}