#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	ComponentInspectorPannel.h
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

	class ComponentInspectorPannel : public ScenePannel
	{
	public:
		ComponentInspectorPannel() = default;
		~ComponentInspectorPannel() = default;

		void OnImGUIRender();
	private:

		ComponentInspectorPannel(ComponentInspectorPannel const&) = delete;
		ComponentInspectorPannel& operator=(ComponentInspectorPannel const&) = delete;


		bool m_ShowCreationMenu = false;

		std::string m_ScriptName = "New Script";
	};

}