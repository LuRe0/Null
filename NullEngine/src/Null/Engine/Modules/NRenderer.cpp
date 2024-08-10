
//------------------------------------------------------------------------------
//
// File Name:	NRenderer.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------
/*
Batch Rendering Code adapted from https://www.youtube.com/watch?v=biGF6oLxgtQ&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=61
*/
//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NRenderer.h"
#include "Null/Tools/Trace.h"
#include <nlohmann/json.hpp>
#include "glad/glad.h"
#include "Null/Engine/Submodules/Graphics/Shader/Shader.h"
#include "Null/Engine/Submodules/Graphics/SpriteSource.h"
#include "Null/Engine/Submodules/Graphics/Mesh.h"
#include "Null/Engine/Submodules/Graphics/Buffers/Framebuffer.h"
#include "imgui.h"
#include "magic_enum/magic_enum.hpp"
#include <sol/sol.hpp>

using JSON = nlohmann::json;

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{

	void NRenderer::Load()
	{

	}

	void NRenderer::BeginRender()
	{

		//for (auto& fb : m_Framebuffers)
		//{
		//	fb.second.Resize(m_WinWidth, m_WinHeight);
		//}

		m_Framebuffers.at("Scene").Bind();

		ClearRender();

		int nean = -1;
		m_Framebuffers.at("Scene").ClearColorAttachment(1, &nean);

		BeginBatch();
	}

	void NRenderer::BeginBatch()
	{
		m_RenderStorage.QuadInstanceBuffer.clear();
		m_RenderStorage.QuadIndexCount = 0;
		m_RenderStorage.TextureSlotIndex = 0;
		m_RenderStorage.TexturesUsed.clear();
	}

	void NRenderer::RenderScene(const RenderData* renderData)
	{
		m_RenderStorage.RenderType = renderData->m_Type;

		//SetBlendMode(BlendMode::DEFAULT);
		renderData->m_Type == RenderData::ELEMENT ?
			RenderElement(*(static_cast<const ElementData*>(renderData))) :
		RenderInstances(*(static_cast<const ElementData*>(renderData)));
	}

	void NRenderer::RenderElement(const ElementData& render)
	{
		NShaderManager* shaderMan = NEngine::Instance().Get<NShaderManager>();
		NCameraManager* cameraManager = NEngine::Instance().Get<NCameraManager>();

		std::string shaderName = render.shaderName.empty() ? "default" : render.shaderName;

		Shader* shader = shaderMan->Get(shaderName);


		shader->Bind();


		Camera* camera = cameraManager->GetCurrentCamera();

		NLE_CORE_ASSERT(camera != nullptr, "No valid camera in use");


		glm::mat4 projection = camera->GetProjectionMatrix();


		glm::mat4 view = camera->GetViewMatrix();


		shader->setFullTransform(render.model, view, projection);


		shader->setVec4("tintColor", render.tintColor);

		shader->setInt("entityID", render.entity);

		if (render.spriteSrc && render.spriteSrc->GetTexture())
		{
			//render.spriteSrc->GetTexture()->Bind();
			glm::vec2 uv = render.spriteSrc->GetUV(render.frameIndex);
			glm::vec2 textureSize = render.spriteSrc->GetSize();
			shader->setVec2("texOffset", uv);
			shader->setVec2("texSize", textureSize);
		}

		if (render.mesh)
		{
			if (render.mesh->GetName() == "Quad")
			{
				m_RenderStorage.Stats.QuadCount++;
			}

			render.mesh->Render(render.spriteSrc);

			m_RenderStorage.TexturesUsed.insert(render.spriteSrc->GetTexture()->GetID());

			m_RenderStorage.Stats.DrawCalls++;

		}

		shader->Unbind();
	}

	void NRenderer::RenderInstances(const ElementData& render)
	{

		NTextureManager* texMan = NEngine::Instance().Get<NTextureManager>();

		if (m_RenderStorage.QuadIndexCount >= m_RenderStorage.MaxIndices)
			NextBatch();

		if (m_RenderStorage.TextureSlotIndex >= m_RenderStorage.MaxTextureSlots)
			NextBatch();

		int textureIndex = -1;
		if (render.spriteSrc)
		{

			for (size_t i = 0; i < m_RenderStorage.TextureSlotIndex; i++)
			{
				if (render.spriteSrc)
				{
					if (m_RenderStorage.TextureSlots[i]->GetID() == render.spriteSrc->GetTexture()->GetID())
					{
						textureIndex = i;
						break;
					}
				}
			}

			if (textureIndex < 0)
			{
				textureIndex = m_RenderStorage.TextureSlotIndex;
				m_RenderStorage.TextureSlots[m_RenderStorage.TextureSlotIndex] = render.spriteSrc->GetTexture();
				++m_RenderStorage.TextureSlotIndex;
			}
		}

		if (render.mesh->GetName() == "Quad")
		{
			const auto& verts = render.mesh->Vertices();


			for (size_t i = 0; i < verts.size(); i++)
			{
				m_RenderStorage.QuadInstanceBuffer.push_back({ verts[i].position, verts[i].color, verts[i].textCoords, render.model, render.tintColor,
				render.spriteSrc != nullptr ? render.spriteSrc->GetUV(render.frameIndex) : glm::vec2(0,0),
				render.spriteSrc != nullptr ? render.spriteSrc->GetSize() : glm::vec2(1,1),
				render.spriteSrc != nullptr ? textureIndex : -1,
				int(render.entity) });
			}

			m_RenderStorage.QuadIndexCount += 6;

			m_RenderStorage.Stats.QuadCount++;
		}
	}

	void NRenderer::EndRender()
	{
		m_RenderQueue.clear();

		Flush();

		m_Framebuffers.at("Scene").Unbind();


		RenderToScreen();
	}

	void NRenderer::NextBatch()
	{
		Flush();
		BeginBatch();
	}

	void NRenderer::Flush()
	{

		if (m_RenderStorage.QuadInstanceBuffer.empty()) return;

		m_RenderStorage.m_QuadInstanceMesh.get()->UpdateInstances(m_RenderStorage.QuadInstanceBuffer, m_RenderStorage.QuadInstanceBuffer.size());

	

		NShaderManager* shaderMan = NEngine::Instance().Get<NShaderManager>();
		NCameraManager* cameraManager = NEngine::Instance().Get<NCameraManager>();
		NTextureManager* textureMan = NEngine::Instance().Get<NTextureManager>();

		std::string shaderName = "objInstance";

		Shader* shader = shaderMan->Get(shaderName);




		Camera* camera = cameraManager->GetCurrentCamera();

		NLE_CORE_ASSERT(camera != nullptr, "No valid camera in use");


		glm::mat4 projection = camera->GetProjectionMatrix();


		glm::mat4 view = camera->GetViewMatrix();
	
	
		// Use shader program and set uniform locations
		shader->Bind();

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, m_RenderStorage.TextureSlots[0]->GetID());

		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, m_RenderStorage.TextureSlots[1]->GetID());

		for (size_t i = 0; i < m_RenderStorage.TextureSlotIndex; i++)
		{
			m_RenderStorage.TextureSlots[i]->BindUnit(i);
		}


		shader->setMat4("view", view);
		shader->setMat4("projection", projection);

		//glUniform1i(glGetUniformLocation(shaderProgram, "u_Texture0"), 0); // Texture unit 0
		//glUniform1i(glGetUniformLocation(shaderProgram, "u_Texture1"), 1); // Texture unit 1
		//
		//shader->setInt("u_Texture0", 0);
		//shader->setInt("u_Texture1", 1);


		m_RenderStorage.m_QuadInstanceMesh.get()->Render(m_RenderStorage.QuadIndexCount);

		m_RenderStorage.Stats.DrawCalls++;
		m_RenderStorage.Stats.TextureCount += m_RenderStorage.TextureSlotIndex;

		shader->Unbind();

		//EndBatch();

	}

	void NRenderer::ResetStats()
	{
		m_RenderStorage.Stats.DrawCalls = 0;
		m_RenderStorage.Stats.QuadCount = 0;
		m_RenderStorage.Stats.TextureCount = 0;
	}

	const NRenderer::RenderStorage::RendererStats& NRenderer::GetStats() const
	{
		// TODO: insert return statement here
		return m_RenderStorage.Stats;
	}

	void NRenderer::RenderToScreen()
	{
		if (!m_Parent->GetIsEditorEnabled())
		{
			ClearRender();

			NShaderManager* shaderMan = NEngine::Instance().Get<NShaderManager>();
			NMeshManager* meshManager = NEngine::Instance().Get<NMeshManager>();
			NCameraManager* cameraManager = NEngine::Instance().Get<NCameraManager>();

			Shader* shader = shaderMan->Get("framebuffer");
			Mesh* mesh = meshManager->Get("Quad");

			shader->Bind();

			Camera* camera = cameraManager->GetCamera<Camera2D>("Default2D");


			glm::mat4 projection = camera->GetProjectionMatrix();

			const auto translate = glm::mat4(1.0f);
			glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(m_WinWidth, -m_WinHeight, 1));;

			shader->setFullTransform(translate * scale, glm::mat4(1.0f), projection);

			shader->setInt("screenTexture", 0);

			uint32_t texture = m_Framebuffers.at("Scene").GetColorAttachment();
			mesh->RenderTexture(texture);

			shader->Unbind();
		}
	}

	void NRenderer::Init()
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();
		NWindow* window = NEngine::Instance().Get<NWindow>();
		SUBSCRIBE_EVENT(WindowResizeEvent, &NRenderer::OnWindowResize, eventManager, EventPriority::Low);

		m_WinWidth = window->Width();
		m_WinHeight = window->Height();


		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);


		m_RenderStorage.m_QuadInstanceMesh = std::make_unique<InstanceMesh>("Quad", m_RenderStorage.MaxVertices, m_RenderStorage.MaxIndices);
		m_RenderStorage.QuadInstanceBuffer.reserve(m_RenderStorage.MaxVertices);

		m_Framebuffers.insert(std::make_pair("Scene", Framebuffer(m_WinWidth, m_WinHeight)));

		Framebuffer& buffer = m_Framebuffers.at("Scene");

		buffer.Init();

		buffer.AddColorAttachment({ Framebuffer::Format(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE), Framebuffer::Format(GL_R32I, GL_RED_INTEGER, GL_INT) });


		std::vector<int32_t> samplers(m_RenderStorage.MaxTextureSlots);

		for (int32_t i = 0; i < samplers.size(); i++)
		{
			samplers[i] = i;
		}


		NShaderManager* shaderMan = NEngine::Instance().Get<NShaderManager>();

		Shader* shader = shaderMan->Get("objInstance");

		shader->Bind();

		shader->setInt1fv("u_Textures", m_RenderStorage.MaxTextureSlots, samplers.data());

		shader->Unbind();

	}

	void NRenderer::Update(float dt)
	{
		ResetStats();
	}

	void NRenderer::RuntimeUpdate(float dt)
	{
	}

	void NRenderer::Render()
	{
		BeginRender();

		for (size_t i = 0; i < m_RenderQueue.size(); i++)
		{
			RenderScene(m_RenderQueue[i].get());
		}

		EndRender();
	}

	void NRenderer::RenderImGui()
	{

		ImGui::Text("Render Type: %s", magic_enum::enum_name(m_RenderStorage.RenderType).data());
		ImGui::Text("Draw Calls: %d", m_RenderStorage.Stats.DrawCalls);
		ImGui::Text("Quads: %d", m_RenderStorage.Stats.QuadCount);
		ImGui::Text("Vertices Calls: %d", m_RenderStorage.Stats.GetTotalVertexCount());
		ImGui::Text("Indices Calls: %d", m_RenderStorage.Stats.GetTotalIndexCount());

		if(m_RenderStorage.RenderType == RenderData::INSTANCED)
			ImGui::Text("Textures Rendered: %d", m_RenderStorage.Stats.TextureCount);
		else
			ImGui::Text("Textures Rendered: %d", m_RenderStorage.TexturesUsed.size());

	}

	void NRenderer::Unload()
	{
	}

	void NRenderer::Shutdown()
	{
		for (auto& fb : m_Framebuffers)
		{
			fb.second.Shutdown();
		}
	}

	void NRenderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void NRenderer::AddRenderCall(std::unique_ptr<RenderData>&& render)
	{
		m_RenderQueue.push_back(std::move(render));
	}

	//void NRenderer::AddElementRenderCall(const ElementData& render)
	//{
	//	m_RenderQueue.push_back(render);
	//}

	//void NRenderer::AddInstancedRenderCall(std::unique_ptr<IEventHandler>&& render)
	//{
	//	m_RenderQueue.push_back(render);
	//}

	const Framebuffer& NRenderer::GetFramebuffer(const std::string& buffer) const
	{
		// TODO: insert return statement here
		return m_Framebuffers.at(buffer);
	}

	void NRenderer::ResizeFramebuffer(unsigned int width, unsigned int height)
	{
		for (auto& fb : m_Framebuffers)
		{
			fb.second.Resize(width, height);
		}
	}

	void NRenderer::ClearRender(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void NRenderer::ClearRenderS()
	{
		glClearColor(.1, .1, .1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void NRenderer::OnWindowResize(const WindowResizeEvent& e)
	{
		if (!m_Parent->GetIsEditorEnabled())
		{
			m_WinWidth = e.GetWidth();
			m_WinHeight = e.GetHeight();

			for (auto& fb : m_Framebuffers)
			{
				fb.second.Resize(m_WinWidth, m_WinHeight);
			}

			SetViewport(0, 0, m_WinWidth, m_WinHeight);
		}
	}
}