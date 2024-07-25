#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NRenderer.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include "Null/Engine/Submodules/Events/IEvents.h"
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

	//class NLE_API Scene;

	class NLE_API NRenderer : public IModule
	{
	public:
		NRenderer() = default;
		~NRenderer() = default;

		/// <summary>
		/// load in scene information
		/// </summary>
		void Load() override;

		void BeginRender();
		void RenderScene(const RenderData& render);
		void EndRender();


		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;

		//! render function
		void Render() override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		void AddRenderCall(const RenderData& render);

		const Framebuffer& GetFramebuffer(const std::string& buffer) const;

		void ClearRender(float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 1.0f);
		static void ClearRenderS();
	private:

		void OnWindowResize(const WindowResizeEvent& e);

		std::vector<RenderData> m_RenderQueue;

		std::unordered_map<std::string, Framebuffer> m_Framebuffers;

		NRenderer(NRenderer const&) = delete;
		NRenderer& operator=(NRenderer const&) = delete;


		float m_WinWidth;
		float m_WinHeight;

	};

}