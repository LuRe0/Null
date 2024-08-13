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
#include <Null/Engine/Submodules/Graphics/Buffers/RenderData.h>
#include <Null/Engine/Submodules/Graphics/Buffers/BatchRenderer/BatchRenderer.h>


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

	

		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;
		void RuntimeUpdate(float dt) override;

		//! render function
		void Render() override;

		void RenderImGui() override;


		void RegisterToScripAPI(sol::state& lua) override {};


		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		void AddRenderCall(std::unique_ptr<ElementData>&& render);

		bool HasRenderImGui() const override { return true; }


		Framebuffer& GetFramebuffer(const std::string& buffer);

		void ResizeFramebuffer(unsigned int width, unsigned int height);

		void ClearRender(float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 1.0f);
		static void ClearRenderS();
	private:
		struct RenderStorage
		{

			RenderData::RenderType RenderType;

		};
		

		RenderStorage m_RenderStorage;

		void OnWindowResize(const WindowResizeEvent& e);

		struct RenderDataComparator {
			bool operator()(const std::unique_ptr<ElementData>& a, const std::unique_ptr<ElementData>& b) const {
				return a->depth < b->depth;  // For sorting in descending order (back to front)
			}
		};

		std::priority_queue<
			std::unique_ptr<ElementData>,
			std::vector<std::unique_ptr<ElementData>>,
			RenderDataComparator
		> m_RenderQueue;

		//std::vector<std::unique_ptr<ElementData>> m_RenderQueue;

		std::unordered_map<std::string, Framebuffer> m_Framebuffers;
		std::unordered_map<std::string, std::unique_ptr<BatchRenderer>> m_Batchers;

		NRenderer(NRenderer const&) = delete;
		NRenderer& operator=(NRenderer const&) = delete;


		void BeginRender();
		void RenderScene(const ElementData* renderData);
		void RenderElement(const ElementData& renderData);
		void RenderInstances(const ElementData& renderData);

		void EndRender();
		void Flush();

		void RenderToScreen();

		float m_WinWidth;
		float m_WinHeight;

	};

}