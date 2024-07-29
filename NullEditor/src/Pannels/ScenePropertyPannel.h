#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	ScenePropertyPannel.h
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

	class ScenePropertyPannel : public Pannel
	{
	public:
		ScenePropertyPannel() = default;
		~ScenePropertyPannel() = default;

		void OnImGUIRender();
	private:
		ScenePropertyPannel(ScenePropertyPannel const&) = delete;
		ScenePropertyPannel& operator=(ScenePropertyPannel const&) = delete;
	};

}