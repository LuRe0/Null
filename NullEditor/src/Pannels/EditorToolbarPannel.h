#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	EditorToolbarPannel.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Submodules/Layers/ImGuiLayer.h"
#include "Null/Engine/Submodules/Graphics/Texture.h"
#include "spdlog/sinks/base_sink.h"
#include "spdlog/spdlog.h"

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




	class EditorToolbarPannel : public Pannel
	{
	public:
		EditorToolbarPannel();
		~EditorToolbarPannel() =  default;

		void OnImGUIRender();
	private:
		enum WindowMode
		{
			MODE_MAXIMIZED,
			MODE_WINDOWED
		};
		EditorToolbarPannel(EditorToolbarPannel const&) = delete;
		EditorToolbarPannel& operator=(EditorToolbarPannel const&) = delete;

		Texture m_PlayButton;
		Texture m_StopButton;

		WindowMode m_CurrentMode = MODE_WINDOWED; // Default mode
	};





}