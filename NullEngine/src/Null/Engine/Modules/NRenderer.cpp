
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

using JSON = nlohmann::json;

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//


void NULLENGINE::NRenderer::Load()
{

}

void NULLENGINE::NRenderer::BeginRender()
{
	m_Framebuffers.at("Scene").Bind();

	glClearColor(0.1f, 0.1f, .1f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void NULLENGINE::NRenderer::RenderScene(const RenderData& render)
{
	//SetBlendMode(BlendMode::DEFAULT);

	NShaderManager* shaderMan = NEngine::Instance().Get<NShaderManager>();
	NCameraManager* cameraManager = NEngine::Instance().Get<NCameraManager>();
	
	std::string shaderName = render.shaderName.empty() ? "default" : render.shaderName;

	Shader* shader = shaderMan->GetShader(shaderName);


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
		glm::vec2 uv = render.spriteSrc->SpriteSourceGetUV(render.frameIndex);
		shader->setVec2("texOffset", uv);
	}

	if (render.mesh)
	{
		render.mesh->Render(render.spriteSrc);
	}

	shader->Unbind();
}

void NULLENGINE::NRenderer::EndRender()
{
	m_Framebuffers.at("Scene").Unbind();

	m_RenderQueue.clear();
}

void NULLENGINE::NRenderer::Init()
{
	NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();
	NWindow* window = NEngine::Instance().Get<NWindow>();
	SUBSCRIBE_EVENT(WindowResizeEvent, &NRenderer::OnWindowResize, eventManager);

	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


	m_Framebuffers.insert(std::make_pair("Scene", Framebuffer(window->Width(), window->Height())));
}

void NULLENGINE::NRenderer::Update(float dt)
{
}

void NULLENGINE::NRenderer::Render()
{
	BeginRender();

	for (size_t i = 0; i < m_RenderQueue.size(); i++)
	{
		RenderScene(m_RenderQueue[i]);
	}

	EndRender();
}

void NULLENGINE::NRenderer::Unload()
{
}

void NULLENGINE::NRenderer::Shutdown()
{
}

void NULLENGINE::NRenderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	glViewport(x, y, width, height);
}

void NULLENGINE::NRenderer::AddRenderCall(const RenderData& render)
{
	m_RenderQueue.push_back(render);
}

void NULLENGINE::NRenderer::ClearRender()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void NULLENGINE::NRenderer::OnWindowResize(const WindowResizeEvent& e)
{
	SetViewport(0, 0, e.GetWidth(), e.GetHeight());
}
