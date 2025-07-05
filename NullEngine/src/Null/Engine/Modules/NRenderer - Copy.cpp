
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
#include "Null/Engine/Submodules/Graphics/Buffers/BatchRenderer/ParticleBatchRenderer.h"
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
	NRenderer::NRenderer() : m_WinHeight(0), m_WinWidth(0)
	{
	}
	void NRenderer::Load()
	{


		std::string filePath = std::string("../Assets/Renderpasses/") + std::string("renderpassses") + std::string(".json");

		std::ifstream file(filePath);
		if (!file.is_open())
		{
			NLE_CORE_ERROR("Failed to load renderpasses.json");
			return;
		}

		nlohmann::json jsonPasses;
		file >> jsonPasses;

		for (const auto& j : jsonPasses)
		{
			RenderPass pass;

			JsonReader reader(j);
			pass.Load(reader);


			m_RenderPasses.push_back(pass);
		}


		NLE_CORE_INFO("Loaded {} render passes", m_RenderPasses.size());
	}

	void NRenderer::BeginRender()
	{
		//NFramebufferManager* fbMan = NEngine::Instance().Get<NFramebufferManager>();

		//m_CurrentFramebuffer->Bind();

		ClearRender();

		std::sort(m_RenderQueue.begin(), m_RenderQueue.end(), RenderDataComparator());

		for (auto& batch : m_Batchers)
			batch.second.get()->BeginBatch();
	}


	void NRenderer::RenderScene(const ElementData* renderData)
	{
		//SetBlendMode(BlendMode::DEFAULT);
		//renderData->m_Type == RenderData::ELEMENT ?
		//	RenderElement(*(renderData)) :
		//	RenderInstances(*(renderData));
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

	void NRenderer::RenderInstances(const ElementData& render, const RenderPass& pass)
	{
		if (!render.mesh)
			return;

		m_Batchers[render.mesh->GetName()].get()->AddInstance(render, pass.shader);
	}

	void NRenderer::RenderParticles(const ParticleData* renderData, const RenderPass& pass)
	{
		static_cast<ParticleBatchRenderer<Mesh>*>(m_Batchers["Particle"].get())->AddInstance(*renderData, pass.shader);
	}

	void NRenderer::EndRender()
	{
		m_RenderQueue.clear();



		//m_CurrentFramebuffer->Unbind();


		//RenderToScreen();
	}


	void NRenderer::Flush(Shader* shader)
	{
		for (auto& batch : m_Batchers)
			batch.second.get()->Flush(shader);
	}

	void NRenderer::RenderToScreen(const RenderPass& pass)
	{
		if (!m_Parent->GetIsEditorEnabled())
		{
			ClearRender();

			NShaderManager* shaderMan = NEngine::Instance().Get<NShaderManager>();
			NMeshManager* meshManager = NEngine::Instance().Get<NMeshManager>();
			NCameraManager* cameraManager = NEngine::Instance().Get<NCameraManager>();
			NFramebufferManager* fbMan = NEngine::Instance().Get<NFramebufferManager>();

			Shader* shader = pass.shader;
			Mesh* mesh = meshManager->Get("Quad");

			shader->Bind();

			Camera* camera = cameraManager->GetCamera<Camera2D>("Default2D");

			glm::mat4 projection = camera->GetProjectionMatrix();

			const auto translate = glm::mat4(1.0f);
			glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(m_WinWidth, -m_WinHeight, 1));

			shader->setFullTransform(translate * scale, glm::mat4(1.0f), projection);

			// Bind all input textures
			for (int i = 0; i < pass.inputs.size(); ++i)
			{
				Framebuffer* fb = fbMan->Get(pass.inputs[i]);
				if (!fb) continue;

				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, fb->GetColorAttachment());
				pass.shader->setInt("inputs[" + std::to_string(i) + "]", i);
			}

			pass.shader->setInt("inputCount", (int)pass.inputs.size());


			//// If you only expect one input, you can also do:
			//// shader->setInt("screenTexture", 0);

			mesh->Render(); // or RenderTexture if you want to pass texture manually

			// Unbind textures
			for (size_t i = 0; i < pass.inputs.size(); ++i)
			{
				glActiveTexture(GL_TEXTURE0 + (GLuint)i);
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			shader->Unbind();
		}
	}

	void NRenderer::Init()
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();
		NWindow* window = NEngine::Instance().Get<NWindow>();
		NFramebufferManager* fbMan = NEngine::Instance().Get<NFramebufferManager>();
		NShaderManager* shaderManager = NEngine::Instance().Get<NShaderManager>();

		//SUBSCRIBE_EVENT(WindowResizeEvent, &NRenderer::OnWindowResize, eventManager, EventPriority::Low);

		m_WinWidth = static_cast<float>(window->Width());
		m_WinHeight = static_cast<float>(window->Height());


		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT); // Or GL_FRONT, depending on your winding order

		m_Batchers.emplace("Cube", std::make_unique<CubeBatchRenderer<Instance, CubeInstanceMesh>>(10000));
		m_Batchers.emplace("Line", std::make_unique<LineBatchRenderer<Instance, LineInstanceMesh>>(5000));
		m_Batchers.emplace("Triangle", std::make_unique<TriangleBatchRenderer<Instance, TriangleInstanceMesh>>(10000));
		m_Batchers.emplace("Circle", std::make_unique<CircleBatchRenderer<Instance, CircleInstanceMesh>>(10000));
		m_Batchers.emplace("Quad", std::make_unique<QuadBatchRenderer<Instance, QuadInstanceMesh>>(10000));


		m_CurrentFramebuffer = fbMan->Get("Scene");

		//m_Framebuffers.insert(std::make_pair("Scene", Framebuffer(static_cast<unsigned int>(m_WinWidth), static_cast<unsigned int>(m_WinHeight))));

		//Framebuffer& buffer = m_Framebuffers.at("Scene");

		//buffer.Init();

		//buffer.AddColorAttachment({ Framebuffer::Format(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE), Framebuffer::Format(GL_R32I, GL_RED_INTEGER, GL_INT) });


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


	void NRenderer::BeginPass(const RenderPass& pass)
	{
		if (pass.framebuffer)
		{
			pass.framebuffer->Bind();

			int nean = -1;
			pass.framebuffer->ClearColorAttachment(1, &nean);
		}
		else
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // Default framebuffer

		//glViewport(0, 0, (int)m_WinWidth, (int)m_WinHeight);

		if (pass.clear)
			glClearColor(pass.clearColor.r, pass.clearColor.g, pass.clearColor.b, pass.clearColor.a);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set blending, culling modes based on pass
		// (You can expand this as needed)
		switch (pass.blendMode)
		{
		case BlendMode::None: glDisable(GL_BLEND); break;
		case BlendMode::Alpha: glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
		case BlendMode::Additive: glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE); break;
		}

		switch (pass.cullMode)
		{
		case CullMode::None: glDisable(GL_CULL_FACE); break;
		case CullMode::Back: glEnable(GL_CULL_FACE); glCullFace(GL_BACK); break;
		case CullMode::Front: glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); break;
		}
	}

	void NRenderer::EndPass(const RenderPass& pass)
	{
		Flush(pass.shader);

		// Unbind framebuffer if needed
		if (pass.framebuffer)
			pass.framebuffer->Unbind();

		// Unbind shader if needed
		if (pass.shader)
			pass.shader->Unbind();
	}


	void NRenderer::Render()
	{
		BeginRender();


		for (const auto& pass : m_RenderPasses)
		{
			BeginPass(pass);

			if (pass.stage == RenderStage::Final)
			{
				RenderToScreen(pass);
			}
			else
			{
				for (auto& renderData : m_RenderQueue)
					RenderInstances(*renderData, pass);



				EndPass(pass);
			}
		}

		//while (!m_RenderQueue.empty()) 
		//{
		//	// Access the element with the highest priority (greatest depth)
		//	auto& renderData = m_RenderQueue.top();

		//	// Process/render the object
		//	RenderScene(renderData.get());

		//	// Remove the element from the queue
		//	m_RenderQueue.pop();
		//}


		//while (!m_DebugRenderQueue.empty())
		//{
		//	// Access the element with the highest priority (greatest depth)
		//	auto& renderData = m_DebugRenderQueue.top();

		//	// Process/render the object
		//	RenderScene(renderData.get());

		//	// Remove the element from the queue
		//	m_DebugRenderQueue.pop();
		//}



		//while (!m_ParticleRenderQueue.empty())
		//{
		//	// Access the element with the highest priority (greatest depth)
		//	auto& renderData = m_ParticleRenderQueue.top();

		//	// Process/render the object
		//	RenderParticles(renderData.get());

		//	// Remove the element from the queue
		//	m_ParticleRenderQueue.pop();
		//}

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

		ImGui::DragFloat4("Clear Color", m_ClearColor.data(), 0.01f, 0.0f, 1.0f);

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
		m_RenderQueue.emplace_back(std::move(render));
	}

	void NRenderer::AddDebugRenderCall(std::unique_ptr<ElementData>&& render)
	{
		m_DebugRenderQueue.emplace_back(std::move(render));
	}

	void NRenderer::AddParticleRenderCall(std::unique_ptr<ParticleData>&& render)
	{
		m_ParticleRenderQueue.emplace_back(std::move(render));
	}

	//void NRenderer::AddElementRenderCall(const ElementData& render)
	//{
	//	m_RenderQueue.push_back(render);
	//}

	//void NRenderer::AddInstancedRenderCall(std::unique_ptr<IEventHandler>&& render)
	//{
	//	m_RenderQueue.push_back(render);
	//}

	//Framebuffer& NRenderer::GetFramebuffer(const std::string& buffer)
	//{
	//	// TODO: insert return statement here
	//	return m_Framebuffers.at(buffer);
	//}


	void NRenderer::ClearRender()
	{
		glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void NRenderer::ClearRenderS()
	{
		glClearColor(.1f, .1f, .1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	RenderPass& NRenderer::AddRenderPass(const RenderPass& pass)
	{
		m_RenderPasses.push_back(pass);
		return m_RenderPasses.back();
	}
	void NRenderer::ClearRenderPasses()
	{
		m_RenderPasses.clear();
	}

}