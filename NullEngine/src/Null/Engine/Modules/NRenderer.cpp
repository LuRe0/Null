
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
#include "Null/Engine/Submodules/Graphics/Mesh/Mesh.h"
#include "Null/Engine/Submodules/Graphics/Mesh/InstanceMeshes/QuadInstanceMesh.h"
#include "Null/Engine/Submodules/Graphics/Mesh/InstanceMeshes/TriangleInstanceMesh.h"
#include "Null/Engine/Submodules/Graphics/Mesh/InstanceMeshes/CubeInstanceMesh.h"
#include "Null/Engine/Submodules/Graphics/Mesh/InstanceMeshes/CircleInstanceMesh.h"
#include "Null/Engine/Submodules/Graphics/Mesh/InstanceMeshes/LineInstanceMesh.h"
#include "Null/Engine/Submodules/Graphics/Buffers/Framebuffer.h"
#include "Null/Engine/Submodules/Graphics/Buffers/BatchRenderer/QuadBatchRenderer.h"
#include "Null/Engine/Submodules/Graphics/Buffers/BatchRenderer/TriangleBatchRenderer.h"
#include "Null/Engine/Submodules/Graphics/Buffers/BatchRenderer/CubeBatchRenderer.h"
#include "Null/Engine/Submodules/Graphics/Buffers/BatchRenderer/CircleBatchRenderer.h"
#include "Null/Engine/Submodules/Graphics/Buffers/BatchRenderer/LineBatchRenderer.h"
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
		m_Framebuffers.at("Scene").Bind();

		ClearRender();

		int nean = -1;
		m_Framebuffers.at("Scene").ClearColorAttachment(1, &nean);

		for (auto& batch : m_Batchers)
			batch.second.get()->BeginBatch();
	}


	void NRenderer::RenderScene(const ElementData* renderData)
	{
		//SetBlendMode(BlendMode::DEFAULT);
		renderData->m_Type == RenderData::ELEMENT ?
			RenderElement(*(renderData)) :
			RenderInstances(*(renderData));
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
			render.mesh->Render(render.spriteSrc);
		}

		shader->Unbind();
	}

	void NRenderer::RenderInstances(const ElementData& render)
	{
		if (!render.mesh)
			return;

		m_Batchers[render.mesh->GetName()].get()->AddInstance(render);
	}

	void NRenderer::EndRender()
	{
		//m_RenderQueue.clear();

		Flush();

		m_Framebuffers.at("Scene").Unbind();


		RenderToScreen();
	}


	void NRenderer::Flush()
	{
		for (auto& batch : m_Batchers)
			batch.second.get()->Flush();
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
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT); // Or GL_FRONT, depending on your winding order

		m_Batchers.emplace("Cube", std::make_unique<CubeBatchRenderer<Instance, CubeInstanceMesh, 10000>>());
		m_Batchers.emplace("Quad", std::make_unique<QuadBatchRenderer<Instance, QuadInstanceMesh, 10000>>());
		m_Batchers.emplace("Triangle", std::make_unique<TriangleBatchRenderer<Instance, TriangleInstanceMesh, 10000>>());
		m_Batchers.emplace("Circle", std::make_unique<CircleBatchRenderer<Instance, CircleInstanceMesh, 10000>>());
		m_Batchers.emplace("Line", std::make_unique<LineBatchRenderer<Instance, LineInstanceMesh, 5000>>());

		m_Framebuffers.insert(std::make_pair("Scene", Framebuffer(m_WinWidth, m_WinHeight)));

		Framebuffer& buffer = m_Framebuffers.at("Scene");

		buffer.Init();

		buffer.AddColorAttachment({ Framebuffer::Format(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE), Framebuffer::Format(GL_R32I, GL_RED_INTEGER, GL_INT) });

		for (auto& batch : m_Batchers)
			batch.second.get()->BindTextureBuffer();
	}

	void NRenderer::Update(float dt)
	{
		for (auto& batch : m_Batchers)
			batch.second.get()->ResetStats();
	}

	void NRenderer::RuntimeUpdate(float dt)
	{
		Update(dt);
	}

	void NRenderer::Render()
	{
		BeginRender();

		while (!m_RenderQueue.empty()) 
		{
			// Access the element with the highest priority (greatest depth)
			auto& renderData = m_RenderQueue.top();

			// Process/render the object
			RenderScene(renderData.get());

			// Remove the element from the queue
			m_RenderQueue.pop();
		}


		while (!m_DebugRenderQueue.empty())
		{
			// Access the element with the highest priority (greatest depth)
			auto& renderData = m_DebugRenderQueue.top();

			// Process/render the object
			RenderScene(renderData.get());

			// Remove the element from the queue
			m_DebugRenderQueue.pop();
		}

		//for (auto& renderData : m_RenderQueue)
		//{
		//	RenderScene(renderData.get());
		//}

		EndRender();
	}

	void NRenderer::RenderImGui()
	{

		//ImGui::Text("Render Type: %s", magic_enum::enum_name(m_RenderStorage.RenderType).data());
		//ImGui::Text("Draw Calls: %d", m_RenderStorage.Stats.DrawCalls);
		//ImGui::Text("Quads: %d", m_RenderStorage.Stats.QuadCount);
		//ImGui::Text("Vertices Calls: %d", m_RenderStorage.Stats.GetTotalVertexCount());
		//ImGui::Text("Indices Calls: %d", m_RenderStorage.Stats.GetTotalIndexCount());

		//if (m_RenderStorage.RenderType == RenderData::INSTANCED)
		//	ImGui::Text("Textures Rendered: %d", m_RenderStorage.Stats.TextureCount);
		//else
		//	ImGui::Text("Textures Rendered: %d", m_RenderStorage.TexturesUsed.size());
		for (auto& batch : m_Batchers)
		{
			batch.second.get()->ImguiView();
			ImGui::Separator();
		}
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

	void NRenderer::AddRenderCall(std::unique_ptr<ElementData>&& render)
	{
		m_RenderQueue.push(std::move(render));
	}

	void NRenderer::AddDebugRenderCall(std::unique_ptr<ElementData>&& render)
	{
		m_DebugRenderQueue.push(std::move(render));
	}

	//void NRenderer::AddElementRenderCall(const ElementData& render)
	//{
	//	m_RenderQueue.push_back(render);
	//}

	//void NRenderer::AddInstancedRenderCall(std::unique_ptr<IEventHandler>&& render)
	//{
	//	m_RenderQueue.push_back(render);
	//}

	Framebuffer& NRenderer::GetFramebuffer(const std::string& buffer)
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