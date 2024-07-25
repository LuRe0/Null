
//------------------------------------------------------------------------------
//
// File Name:	NRenderer.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

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



	}

	void NRenderer::RenderScene(const RenderData& render) 
	{
		//SetBlendMode(BlendMode::DEFAULT);

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

		if (render.spriteSrc && render.spriteSrc->GetTexture())
		{
			//render.spriteSrc->GetTexture()->Bind();
			glm::vec2 uv = render.spriteSrc->GetUV(render.frameIndex);
			shader->setVec2("texOffset", uv);
		}

		if (render.mesh)
		{
			render.mesh->Render(render.spriteSrc);
		}

		shader->Unbind();
	}

	void NRenderer::EndRender()
	{
		m_RenderQueue.clear();

		m_Framebuffers.at("Scene").Unbind();

		ClearRender();

		NShaderManager* shaderMan = NEngine::Instance().Get<NShaderManager>();
		NMeshManager* meshManager = NEngine::Instance().Get<NMeshManager>();
		NCameraManager* cameraManager = NEngine::Instance().Get<NCameraManager>();

		Shader* shader = shaderMan->Get("framebuffer");
		Mesh* mesh = meshManager->Get("Quad1x1");

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

	void NRenderer::Init()
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();
		NWindow* window = NEngine::Instance().Get<NWindow>();
		SUBSCRIBE_EVENT(WindowResizeEvent, &NRenderer::OnWindowResize, eventManager);

		m_WinWidth = window->Width();
		m_WinHeight = window->Height();


		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		m_Framebuffers.insert(std::make_pair("Scene", Framebuffer(m_WinWidth, m_WinHeight)));

		for (auto& fb : m_Framebuffers)
		{
			fb.second.Init();
		}

		SetViewport(0, 0, m_WinWidth, m_WinHeight);
	}

	void NRenderer::Update(float dt)
	{
	}

	void NRenderer::Render()
	{
		BeginRender();

		for (size_t i = 0; i < m_RenderQueue.size(); i++)
		{
			RenderScene(m_RenderQueue[i]);
		}

		EndRender();
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

	void NRenderer::AddRenderCall(const RenderData& render)
	{
		m_RenderQueue.push_back(render);
	}

	const Framebuffer& NRenderer::GetFramebuffer(const std::string& buffer) const
	{
		// TODO: insert return statement here
		return m_Framebuffers.at(buffer);
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
		m_WinWidth = e.GetWidth();
		m_WinHeight = e.GetHeight();

		for (auto& fb : m_Framebuffers)
		{
			fb.second.Resize(m_WinWidth, m_WinHeight);
		}

		SetViewport(0, 0, m_WinWidth, m_WinHeight);
	}
}