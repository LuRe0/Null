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
#include <Null/Engine/Submodules/Graphics/Texture1D.h>

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
		void NextBatch(Shader* shader);
		void Flush(Shader* shader);
		void AddInstance(const ElementData& render, Shader* shader);
		void AddInstance(const ParticleData& render, Shader* shader);
		void UploadTextureIndexBuffer();
		void ImguiView();
		void ResetStats();
		void BindTextureBuffer(Shader* shader = nullptr);

		void SetSSBO(SSBO& inSSBO);
		void SetParticleCount(size_t count);

	private:
		std::unique_ptr<TMesh> m_InstanceMesh;

		RendererStats m_Stats;

		Texture1D m_TextureIndexTexture;
		std::vector<int> m_TextureIndexLookup; // CPU-side copy
	};


	template <typename TMesh>
	inline void ParticleBatchRenderer<TMesh>::BeginBatch()
	{
		m_InstanceIndexCount = 0;
		m_TextureSlotIndex = 0;
		m_TextureIndexLookup.clear();
	}

	template <typename TMesh>
	inline void ParticleBatchRenderer<TMesh>::NextBatch(Shader* shader)
	{
		Flush(shader);
		BeginBatch();
	}

	template <typename TMesh>
	void ParticleBatchRenderer<TMesh>::SetSSBO(SSBO& inSSBO)
	{
		m_InstanceMesh.get()->SetSSBO(inSSBO);
	}

	template <typename TMesh>
	void ParticleBatchRenderer<TMesh>::Flush(Shader* inShader)
	{
		auto* shaderMan = NShaderManager::Instance();
		auto* cameraManager = NCameraManager::Instance();

		Camera* camera = cameraManager->GetCurrentCamera();
		NLE_CORE_ASSERT(camera != nullptr, "No valid camera in use");

		UploadTextureIndexBuffer();
		Shader* shader = shaderMan->Get("particle");
		shader->Bind();

		int texBufferUnit = 31; // Reserved slot for texture index lookup

		m_TextureIndexTexture.BindUnit(texBufferUnit);
		shader->setInt("u_TextureIDToSlot", texBufferUnit);


		for (size_t i = 0; i < m_TextureSlotIndex; i++)
		{	
			if(m_TextureSlots[i])
				m_TextureSlots[i]->BindUnit(static_cast<uint32_t>(i));
		}

		shader->setMat4("view", camera->GetViewMatrix());
		shader->setMat4("projection", camera->GetProjectionMatrix());

		// Render, supplying how many instances/particles to draw
		// Assuming 6 indices per quad per particle
		m_InstanceMesh->RenderInstanced();

		m_Stats.DrawCalls++;


		shader->Unbind();
	}

	template <typename TMesh>
	void ParticleBatchRenderer<TMesh>::SetParticleCount(size_t count)
	{
		m_InstanceIndexCount = count;
	}

	template <typename TMesh>
	inline void ParticleBatchRenderer<TMesh>::AddInstance(const ElementData& render, Shader* shader)
	{

	}

	template<typename TMesh>
	inline void ParticleBatchRenderer<TMesh>::AddInstance(const ParticleData& render, Shader* shader)
	{
		int textureIndex = -1;
		if (render.spriteSrc)
		{

			if (m_TextureSlotIndex >= m_MaxTextureSlots-1)
				NextBatch(shader);

			auto* tex = render.spriteSrc->GetTexture();
			uint32_t compactID = NEngine::Instance()
				.Get<NTextureManager>()->GetTextureIndex(render.spriteSrc->GetName());

			for (size_t i = 0; i < m_TextureSlotIndex; i++)
			{
				if (render.spriteSrc)
				{
					if (m_TextureSlots[i]->GetID() == render.spriteSrc->GetTexture()->GetID())
					{
						textureIndex = static_cast<int>(i);
						break;
					}
				}
			}

			if (textureIndex < 0)
			{
				textureIndex = m_TextureSlotIndex;
				m_TextureSlots[m_TextureSlotIndex] = render.spriteSrc->GetTexture();
				++m_TextureSlotIndex;
			}


			if (compactID >= m_TextureIndexLookup.size())
				m_TextureIndexLookup.resize(compactID + 1, -1);

			m_TextureIndexLookup[compactID] = textureIndex;
		}
	}

	template <typename TMesh>
	void ParticleBatchRenderer<TMesh>::UploadTextureIndexBuffer()
	{
		if (m_TextureIndexLookup.empty())
			return;

		int newSize = static_cast<int>(m_TextureIndexLookup.size());


		if (m_TextureIndexTexture.GetID() == 0 || m_TextureIndexTexture.GetSize() != newSize)
			m_TextureIndexTexture.Init(newSize);


		m_TextureIndexTexture.Update(m_TextureIndexLookup.data());
	}

	template <typename TMesh>
	inline void ParticleBatchRenderer<TMesh>::ImguiView()
	{
		if (!m_Stats.DrawCalls) return;

		ImGui::Text("Render Type: %s", "PARTICLES");
		ImGui::Text("Draw Calls: %d", m_Stats.DrawCalls);
		ImGui::Text("Quads: %d", m_Stats.InstanceCount);
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
	inline void ParticleBatchRenderer<TMesh>::BindTextureBuffer(Shader* inShader)
	{
		std::vector<int32_t> samplers(BatchRenderer::m_MaxTextureSlots-1);

		for (int32_t i = 0; i < samplers.size(); i++)
		{
			samplers[i] = i;
		}


		NShaderManager* shaderMan = NShaderManager::Instance();

		Shader* shader = shaderMan->Get("particle");

		shader->Bind();

		shader->setInt1fv("u_Textures", BatchRenderer::m_MaxTextureSlots-1, samplers.data());

		shader->Unbind();
	}

}