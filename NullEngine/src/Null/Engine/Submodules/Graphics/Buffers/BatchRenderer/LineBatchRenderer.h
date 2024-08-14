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
	template <typename TInstance, typename TMesh, std::size_t TCount>
	class LineBatchRenderer : public BatchRenderer
	{
	public:
		LineBatchRenderer()
		{
			m_MaxInstances = TCount;
			m_MaxVertices = m_MaxInstances;
			m_MaxIndices = m_MaxInstances * 2;

			m_InstanceMesh = std::make_unique<TMesh>("Line", m_MaxVertices, m_MaxIndices);
			m_InstanceBuffer.reserve(m_MaxVertices);
		}

		~LineBatchRenderer() override
		{
		}

		void BeginBatch();
		void NextBatch();
		void Flush();
		void AddInstance(const ElementData& render);
		void ImguiView();
		void ResetStats();
		void BindTextureBuffer();

	private:
		std::vector<TInstance> m_InstanceBuffer;
		std::unique_ptr<TMesh> m_InstanceMesh;

		RendererStats m_Stats;
	};


	template<typename TInstance, typename TMesh, std::size_t TCount>
	inline void LineBatchRenderer<TInstance, TMesh, TCount>::BeginBatch()
	{
		m_InstanceBuffer.clear();
		m_InstanceIndexCount = 0;
		m_TextureSlotIndex = 0;
	}

	template<typename TInstance, typename TMesh, std::size_t TCount>
	inline void LineBatchRenderer<TInstance, TMesh, TCount>::NextBatch()
	{
		Flush();
		BeginBatch();
	}

	template<typename TInstance, typename TMesh, std::size_t TCount>
	inline void LineBatchRenderer<TInstance, TMesh, TCount>::Flush()
	{
		if (m_InstanceBuffer.empty()) return;


		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendEquation(GL_FUNC_ADD);
		//glDisable(GL_DEPTH_TEST);
		//glDepthMask(GL_FALSE);  // Disable depth writing

		m_InstanceMesh.get()->UpdateInstances(m_InstanceBuffer, m_InstanceBuffer.size());

		NShaderManager* shaderMan = NEngine::Instance().Get<NShaderManager>();
		NCameraManager* cameraManager = NEngine::Instance().Get<NCameraManager>();
		NTextureManager* textureMan = NEngine::Instance().Get<NTextureManager>();

		std::string shaderName = "lineInstance";

		Shader* shader = shaderMan->Get(shaderName);

		Camera* camera = cameraManager->GetCurrentCamera();

		NLE_CORE_ASSERT(camera != nullptr, "No valid camera in use");


		glm::mat4 projection = camera->GetProjectionMatrix();


		glm::mat4 view = camera->GetViewMatrix();


		// Use shader program and set uniform locations
		shader->Bind();


		for (size_t i = 0; i < m_TextureSlotIndex; i++)
		{
			m_TextureSlots[i]->BindUnit(i);
		}


		shader->setMat4("view", view);
		shader->setMat4("projection", projection);


		m_InstanceMesh.get()->Render(m_InstanceIndexCount);

		m_Stats.DrawCalls++;
		m_Stats.TextureCount += m_TextureSlotIndex;
	}

	template<typename TInstance, typename TMesh, std::size_t TCount>
	inline void LineBatchRenderer<TInstance, TMesh, TCount>::AddInstance(const ElementData& render)
	{

		if (!render.mesh)
			return;


		NTextureManager* texMan = NEngine::Instance().Get<NTextureManager>();

		if (m_InstanceIndexCount >= m_MaxIndices)
			NextBatch();

		if (m_TextureSlotIndex >= m_MaxTextureSlots)
			NextBatch();

		int textureIndex = -1;
		if (render.spriteSrc)
		{

			for (size_t i = 0; i < m_TextureSlotIndex; i++)
			{
				if (render.spriteSrc)
				{
					if (m_TextureSlots[i]->GetID() == render.spriteSrc->GetTexture()->GetID())
					{
						textureIndex = i;
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
		}


		const auto& verts = render.mesh->Vertices();


		for (size_t i = 0; i < verts.size(); i++)
		{
			m_InstanceBuffer.push_back({ verts[i].position, verts[i].color, verts[i].textCoords, render.model, render.tintColor,
			render.spriteSrc != nullptr ? render.spriteSrc->GetUV(render.frameIndex) : glm::vec2(0,0),
			render.spriteSrc != nullptr ? render.spriteSrc->GetSize() : glm::vec2(1,1),
			render.thickness, render.fade,
			render.spriteSrc != nullptr ? textureIndex : -1,
			int(render.entity) });
		}

		m_InstanceIndexCount += 4;

		m_Stats.InstanceCount++;

	}

	template<typename TInstance, typename TMesh, std::size_t TCount>
	inline void LineBatchRenderer<TInstance, TMesh, TCount>::ImguiView()
	{
		if (!m_Stats.DrawCalls) return;

		ImGui::Text("Render Type: %s", "LINES");
		ImGui::Text("Draw Calls: %d", m_Stats.DrawCalls);
		ImGui::Text("Quads: %d", m_Stats.InstanceCount);
		ImGui::Text("Vertices Calls: %d", m_Stats.InstanceCount);
		ImGui::Text("Indices Calls: %d", m_Stats.InstanceCount * 2);
		ImGui::Text("Textures Rendered: %d", m_Stats.TextureCount);
	}

	template<typename TInstance, typename TMesh, std::size_t TCount>
	inline void LineBatchRenderer<TInstance, TMesh, TCount>::ResetStats()
	{
		m_Stats.DrawCalls = 0;
		m_Stats.InstanceCount = 0;
		m_Stats.TextureCount = 0;
	}

	template<typename TInstance, typename TMesh, std::size_t TCount>
	inline void LineBatchRenderer<TInstance, TMesh, TCount>::BindTextureBuffer()
	{
		std::vector<int32_t> samplers(BatchRenderer::m_MaxTextureSlots);

		for (int32_t i = 0; i < samplers.size(); i++)
		{
			samplers[i] = i;
		}


		NShaderManager* shaderMan = NEngine::Instance().Get<NShaderManager>();

		Shader* shader = shaderMan->Get("lineInstance");

		shader->Bind();

		shader->setInt1fv("u_Textures", BatchRenderer::m_MaxTextureSlots, samplers.data());

		shader->Unbind();
	}

}