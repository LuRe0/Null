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
#include "../Pannels/Base/Pannel.h"

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
	class NDebugManager;
	class NFramebufferManager;




	class EditorToolbarPannel : public ScenePannel
	{
	public:
		EditorToolbarPannel();
		~EditorToolbarPannel() =  default;

		void OnImGUIRender();
		void RenderPlayControls(float buttonSize);
		void RenderDebugControls(NDebugManager* debugManager);
		void RenderRenderSettings(NFramebufferManager* fbMan);
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