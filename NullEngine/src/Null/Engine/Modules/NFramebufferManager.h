#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NWindow.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include "Null/Engine/Submodules/Graphics/Buffers/Framebuffer.h"


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
	class NLE_API NFramebufferManager : public NResourceManager<Framebuffer>
	{
	public:

		void Load() override;
		////! Virtual Init function
		void Init() override;
		////! Virtual Update function
		//void Update(float dt) override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

	private:
		bool OnWindowResize(const WindowResizeEvent& e);


		float m_WinWidth = 0.0f;
		float m_WinHeight = 0.0f;
	};

}