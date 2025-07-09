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
#include <Null/Engine/Submodules/Graphics/Buffers/RenderPass.h>
#include <setjmp.h>


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
	class Shader;

	//class NLE_API Scene;

	class NLE_API NRenderer : public ModuleBase<NRenderer>
	{
	public:
		NRenderer();
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

		void DrawQueue(RenderCommandTypes type, const RenderPass& pass);

		void RenderImGui() override;


		void RegisterToScripAPI(sol::state& lua) override {};


		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		void AddRenderCall(std::unique_ptr<ElementData>&& render);
		void AddDebugRenderCall(std::unique_ptr<ElementData>&& render);
		void AddParticleRenderCall(std::unique_ptr<ParticleData>&& render);

		void AddEmissiveRenderCall(std::unique_ptr<ElementData>&& render);

		void AddRenderCall(RenderCommandTypes type, std::unique_ptr<RenderData>&& data);

		bool HasRenderImGui() const override { return true; }


		Framebuffer& GetFramebuffer(const std::string& buffer);

		void ResizeFramebuffer(unsigned int width, unsigned int height);

		template<typename TBatcher>
		TBatcher* AddBatcher(const std::string& name, std::size_t count);

		template<typename TBatcher>
		TBatcher* AddBatcher(const std::string& name);

		void ClearRender();
		void ClearRender_Params(float r = 0.10f, float g = 0.10f, float b = 0.10f, float a = 1.0f);
		static void ClearRenderS();
	private:

		bool OnWindowResize(const WindowResizeEvent& e);

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

		std::priority_queue<
			std::unique_ptr<ElementData>,
			std::vector<std::unique_ptr<ElementData>>,
			RenderDataComparator
		> m_EmissiveRenderQueue;

		std::priority_queue<
			std::unique_ptr<ElementData>,
			std::vector<std::unique_ptr<ElementData>>,
			RenderDataComparator
		> m_DebugRenderQueue;

		std::priority_queue<
			std::unique_ptr<ParticleData>,
			std::vector<std::unique_ptr<ParticleData>>,
			ParticleDepthCompare
		> m_ParticleRenderQueue;

		std::vector<std::unique_ptr<IRenderQueue>> m_RenderCommands;

		std::vector<RenderPass> m_RenderPasses;



		//std::vector<std::unique_ptr<ElementData>> m_RenderQueue;

		std::unordered_map<std::string, Framebuffer> m_Framebuffers;
		std::unordered_map<std::string, std::unique_ptr<BatchRenderer>> m_Batchers;

		NRenderer(NRenderer const&) = delete;
		NRenderer& operator=(NRenderer const&) = delete;


		void BeginRender();
		void RenderScene(const ElementData* renderData);
		void RenderElement(const ElementData& renderData);
		void RenderInstances(const ElementData* renderData, Shader* shader);
		void RenderParticles(const ParticleData* renderData);

		void EndRender();
		void Flush(const RenderPass& pass);
		void SetRenderState(const RenderPass& pass);
		void SetBlendMode(const RenderPass& pass);

		void RenderCompositePass(const RenderPass& pass);

		void RenderToScreen();

		float m_WinWidth = 0.0f;
		float m_WinHeight = 0.0f;

		std::vector<float> m_ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	};

	template<typename TBatcher>
	inline TBatcher* NRenderer::AddBatcher(const std::string& name, std::size_t count)
	{
		if(!m_Batchers.contains(name))
			m_Batchers.emplace(name, std::make_unique<TBatcher>(count));

		return dynamic_cast<TBatcher*>(m_Batchers[name].get());
	}

	template<typename TBatcher>
	inline TBatcher* NRenderer::AddBatcher(const std::string& name)
	{
		if (!m_Batchers.contains(name))
			m_Batchers.emplace(name, std::make_unique<TBatcher>());

		return dynamic_cast<TBatcher*>(m_Batchers[name].get());
	}
}