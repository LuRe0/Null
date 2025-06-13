#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	BatchRenderer.h
// Author(s):	name
// 
//------------------------------------------------------------------------------
/*
Code adapted from https://learnopengl.com
Made by Joey de Vries https://twitter.com/JoeyDeVriez\
LearnOpenGl license: https://creativecommons.org/licenses/by/4.0/legalcode
*/
//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "BatchRenderer.h"
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
	template <typename TMesh>
	class ParticleBatchRenderer : public BatchRenderer
	{
	public:
		ParticleBatchRenderer()
		{
			m_InstanceMesh = std::make_unique<TMesh>("Quad");
		}

		~ParticleBatchRenderer() override
		{
		}

		void BeginBatch();
		void NextBatch();
		void Flush();
		void AddInstance(const ElementData& render);
		void ImguiView();
		void ResetStats();
		void BindTextureBuffer();

		void SetSSBO(SSBO& inSSBO);
		void SetParticleCount(size_t count);

	private:
		std::unique_ptr<TMesh> m_InstanceMesh;

		RendererStats m_Stats;
	};


	template <typename TMesh>
	inline void ParticleBatchRenderer<TMesh>::BeginBatch()
	{
		m_InstanceIndexCount = 0;
		m_TextureSlotIndex = 0;
	}

	template <typename TMesh>
	inline void ParticleBatchRenderer<TMesh>::NextBatch()
	{
		Flush();
		BeginBatch();
	}

	template <typename TMesh>
	void ParticleBatchRenderer<TMesh>::SetSSBO(SSBO& inSSBO)
	{
		m_InstanceMesh.get()->SetSSBO(inSSBO);
	}

	template <typename TMesh>
	void ParticleBatchRenderer<TMesh>::Flush()
	{
		auto* shaderMan = NEngine::Instance().Get<NShaderManager>();
		auto* cameraManager = NEngine::Instance().Get<NCameraManager>();

		Camera* camera = cameraManager->GetCurrentCamera();
		NLE_CORE_ASSERT(camera != nullptr, "No valid camera in use");

		Shader* shader = shaderMan->Get("particle");
		shader->Bind();

		shader->setMat4("view", camera->GetViewMatrix());
		shader->setMat4("projection", camera->GetProjectionMatrix());

		// Render, supplying how many instances/particles to draw
		// Assuming 6 indices per quad per particle
		m_InstanceMesh->RenderInstanced();

		shader->Unbind();
	}

	template <typename TMesh>
	void ParticleBatchRenderer<TMesh>::SetParticleCount(size_t count)
	{
		m_InstanceIndexCount = count;
	}

	template <typename TMesh>
	inline void ParticleBatchRenderer<TMesh>::AddInstance(const ElementData& render)
	{
	}

	template <typename TMesh>
	inline void ParticleBatchRenderer<TMesh>::ImguiView()
	{
		if (!m_Stats.DrawCalls) return;

		ImGui::Text("Render Type: %s", "QUADS");
		ImGui::Text("Draw Calls: %d", m_Stats.DrawCalls);
		ImGui::Text("Quads: %d", m_Stats.InstanceCount);
		ImGui::Text("Vertices Calls: %d", m_Stats.InstanceCount * 4);
		ImGui::Text("Indices Calls: %d", m_Stats.InstanceCount * 6);
		ImGui::Text("Textures Rendered: %d", m_Stats.TextureCount);
	}

	template <typename TMesh>
	inline void ParticleBatchRenderer<TMesh>::ResetStats()
	{
		m_Stats.DrawCalls = 0;
		m_Stats.InstanceCount = 0;
		m_Stats.TextureCount = 0;
	}

	template <typename TMesh>
	inline void ParticleBatchRenderer<TMesh>::BindTextureBuffer()
	{
		std::vector<int32_t> samplers(BatchRenderer::m_MaxTextureSlots);

		for (int32_t i = 0; i < samplers.size(); i++)
		{
			samplers[i] = i;
		}


		NShaderManager* shaderMan = NEngine::Instance().Get<NShaderManager>();

		Shader* shader = shaderMan->Get("particle");

		shader->Bind();

		shader->setInt1fv("u_Textures", BatchRenderer::m_MaxTextureSlots, samplers.data());

		shader->Unbind();
	}

}