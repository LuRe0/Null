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

		void AddRenderCall(std::unique_ptr<RenderData>&& render);

		bool HasRenderImGui() const override { return true; }

		const uint32_t MaxQuads() { return m_RenderStorage.MaxQuads; }
		const uint32_t MaxVertices() { return m_RenderStorage.MaxVertices; }
		const uint32_t MaxIndices() { return m_RenderStorage.MaxIndices; }

		Framebuffer& GetFramebuffer(const std::string& buffer);

		void ResizeFramebuffer(unsigned int width, unsigned int height);

		void ClearRender(float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 1.0f);
		static void ClearRenderS();
	private:
		struct RenderStorage
		{
			const uint32_t MaxQuads = 10000;
			const uint32_t MaxVertices = MaxQuads * 4;
			const uint32_t MaxIndices = MaxQuads * 6;
			static const uint32_t MaxTextureSlots = 32;


			uint32_t QuadIndexCount = 0;
			uint32_t TextureSlotIndex = 0;

			std::vector<Instance> QuadInstanceBuffer;

			std::array<Texture*, MaxTextureSlots> TextureSlots;

			std::unique_ptr<InstanceMesh> m_QuadInstanceMesh;
			std::unique_ptr<InstanceMesh> m_TriInstanceMesh;
			std::unique_ptr<InstanceMesh> m_CubeInstanceMesh;

			struct RendererStats
			{
				uint32_t DrawCalls = 0;
				uint32_t QuadCount = 0;
				uint32_t TextureCount = 0;

				uint32_t GetTotalVertexCount() { return QuadCount * 4; }
				uint32_t GetTotalIndexCount() { return QuadCount * 6; }
			};

			RendererStats Stats;

			RenderData::RenderType RenderType;

			std::unordered_set<unsigned int> TexturesUsed;
		};
		

		RenderStorage m_RenderStorage;

		void OnWindowResize(const WindowResizeEvent& e);

		std::vector<std::unique_ptr<RenderData>> m_RenderQueue;

		std::unordered_map<std::string, Framebuffer> m_Framebuffers;

		NRenderer(NRenderer const&) = delete;
		NRenderer& operator=(NRenderer const&) = delete;


		void BeginRender();
		void BeginBatch();
		void RenderScene(const RenderData* renderData);
		void RenderElement(const ElementData& renderData);
		void RenderInstances(const ElementData& renderData);
		//void EndBatch();
		void EndRender();
		void NextBatch();
		void Flush();

		void ResetStats();
		const RenderStorage::RendererStats& GetStats() const;

		void RenderToScreen();

		float m_WinWidth;
		float m_WinHeight;

	};

}