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
#include "Null/Engine/Submodules/Layers/ImGuiLayer.h"

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

	class ComponentInspectorPannel : public Pannel
	{
	public:
		ComponentInspectorPannel() = default;
		~ComponentInspectorPannel() = default;

		void OnImGUIRender();
	private:
		Scene* m_Context;

		ComponentInspectorPannel(ComponentInspectorPannel const&) = delete;
		ComponentInspectorPannel& operator=(ComponentInspectorPannel const&) = delete;
	};

}