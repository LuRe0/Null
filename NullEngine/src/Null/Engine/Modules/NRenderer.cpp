
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
		NFramebufferManager* fbMan = NFramebufferManager::Instance();
		Framebuffer* fb = fbMan->Get("Scene");
		fb->Bind();

		ClearRender();

		int nean = -1;
		fb->ClearColorAttachment(1, &nean);

		for (auto& batch : m_Batchers)
			batch.second.get()->BeginBatch();
	}


	void NRenderer::RenderScene(const ElementData* renderData)
	{
		////SetBlendMode(BlendMode::DEFAULT);
		//renderData->m_Type == RenderData::ELEMENT ?
		//	RenderElement(*(renderData)) :
		//	RenderInstances(*(renderData));
	}

	void NRenderer::RenderElement(const ElementData& render)
	{
		NShaderManager* shaderMan = NShaderManager::Instance();
		NCameraManager* cameraManager = NCameraManager::Instance();

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

	void NRenderer::RenderInstances(const ElementData* render, Shader* shader)
	{
		if (!render->mesh)
			return;

		NShaderManager* shaderMan = NShaderManager::Instance();


		m_Batchers[render->mesh->GetName()].get()->AddInstance(*render, shader);
	}

	void NRenderer::RenderParticles(const ParticleData* renderData)
	{
		static_cast<ParticleBatchRenderer<Mesh>*>(m_Batchers["Particle"].get())->AddInstance(*renderData, nullptr);
	}

	void NRenderer::EndRender()
	{
		//m_RenderQueue.clear();

		//Flush(nullptr);

		NFramebufferManager* fbMan = NFramebufferManager::Instance();
		Framebuffer* fb = fbMan->Get("Scene");
		
		fb->Unbind();


		RenderToScreen();
	}


	void NRenderer::Flush(const RenderPass& pass)
	{
		for (const auto& batchName : pass.batchersToFlush)
		{
			auto it = m_Batchers.find(batchName);
			if (it != m_Batchers.end())
			{
				it->second->BindTextureBuffer(pass.shader);
				it->second->Flush(pass.shader);
			}
		}
	}

	void NRenderer::SetRenderState(const RenderPass& pass)
	{
		if (pass.depthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		// Depth write mask
		glDepthMask(pass.depthWrite ? GL_TRUE : GL_FALSE);

		// Depth function
		glDepthFunc(pass.depthFunc);

		// Blend mode
		switch (pass.blendMode)
		{
		case BlendMode::None:
			glDisable(GL_BLEND);
			break;

		case BlendMode::Alpha:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;

		case BlendMode::Additive:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;
		}

		// Cull mode
		switch (pass.cullMode)
		{
		case CullMode::None:
			glDisable(GL_CULL_FACE);
			break;

		case CullMode::Back:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;

		case CullMode::Front:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			break;
		}
	}

	void NRenderer::RenderCompositePass(const RenderPass& pass)
	{
		NShaderManager* shaderMan = NShaderManager::Instance();
		NMeshManager* meshManager = NMeshManager::Instance();
		NFramebufferManager* fbMan = NFramebufferManager::Instance();

		Shader* shader = pass.shader;
		Mesh* mesh = meshManager->Get("Quad");

		shader->Bind();

		Camera* camera = NCameraManager::Instance()->GetCamera<Camera2D>("Default2D");
		Camera* currentCamera = NCameraManager::Instance()->GetCurrentCamera();
		glm::mat4 projection = camera->GetProjectionMatrix();
		glm::vec2 dims = pass.framebuffer->GetSize();

		glm::mat4 translate = glm::mat4(1.0f);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(dims.x, -dims.y, 0.0f));
		shader->setFullTransform(translate * scale, glm::mat4(1.0f), projection);

		// Bind input textures
		for (int i = 0; i < pass.inputs.size(); ++i)
		{
			Framebuffer* fb = fbMan->Get(pass.inputs[i]);
			if (!fb) {
				NLE_CORE_ERROR("Missing framebuffer input: {}", pass.inputs[i]);
				continue;
			}
			uint32_t tex = fb->GetColorAttachment(0);
			if (tex == 0) {
				NLE_CORE_ERROR("Framebuffer has no color attachment: {}", pass.inputs[i]);
				continue;
			}
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, tex);
			pass.shader->setInt("inputs[" + std::to_string(i) + "]", i);
		}

		const PostProcess& pp = currentCamera->GetPPSettings();

		if (pass.name == "BrightPass") {
			shader->setFloat("threshold", pp.BloomThreshold * pp.UseBloom);
			shader->setFloat("intensity", pp.BloomIntensity * pp.UseBloom);
		}
		else if (pass.name == "BlurH") {
			shader->setFloat("texelWidth", 1.0f / dims.x);
		}
		else if (pass.name == "BlurV") {
			shader->setFloat("texelHeight", 1.0f / dims.y);
		}
		else if (pass.name == "Vignette") {
			shader->setFloat("radius", pp.VignetteRadius*pp.UseVignette);
			shader->setFloat("intensity", pp.VignetteIntensity * pp.UseVignette);
		}
		else if (pass.name == "Grayscale") {
			shader->setFloat("grayscaleAmount", pp.GrayscaleAmount * pp.UseGrayscale);
		}
		else if (pass.name == "Tint") {
			shader->setVec4("tintColor", pp.TintColor * static_cast<float>(pp.UseTint));
			shader->setFloat("tintStrength", pp.TintStrength * pp.UseTint);
		}
		else if (pass.name == "Chromatic") {
			shader->setFloat("offset", pp.ChromaticOffset / dims.x * static_cast<float>(pp.UseChromatic)); // pixel offset → UV
		}
		else if (pass.name == "Grain") {
			shader->setFloat("grainAmount", pp.GrainAmount*pp.UseGrain);
			shader->setFloat("time", Time::LastTime()); // or your delta/total time
		}


		mesh->Render();

		// Unbind textures
		for (size_t i = 0; i < pass.inputs.size(); ++i)
		{
			glActiveTexture(GL_TEXTURE0 + static_cast<GLuint>(i));
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		shader->Unbind();
	}

	void NRenderer::RenderToScreen()
	{
		//if (!m_Parent->GetIsEditorEnabled())
		//{
		//	ClearRender();

		//	NShaderManager* shaderMan = NShaderManager::Instance();
		//	NMeshManager* meshManager = NMeshManager::Instance();
		//	NCameraManager* cameraManager = NCameraManager::Instance();

		//	Shader* shader = shaderMan->Get("framebuffer");
		//	Mesh* mesh = meshManager->Get("Quad");

		//	shader->Bind();

		//	Camera* camera = cameraManager->GetCamera<Camera2D>("Default2D");


		//	glm::mat4 projection = camera->GetProjectionMatrix();

		//	const auto translate = glm::mat4(1.0f);
		//	glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(m_WinWidth, -m_WinHeight, 1));;

		//	shader->setFullTransform(translate * scale, glm::mat4(1.0f), projection);

		//	shader->setInt("screenTexture", 0);

		//	uint32_t texture = m_Framebuffers.at("Scene").GetColorAttachment();
		//	mesh->RenderTexture(texture);

		//	shader->Unbind();
		//}
	}

	void NRenderer::Init()
	{
		NEventManager* eventManager =   NEventManager::Instance();
		NWindow* window = NWindow::Instance();
		SUBSCRIBE_EVENT(WindowResizeEvent, &NRenderer::OnWindowResize, eventManager, EventPriority::Low);

		m_WinWidth = static_cast<float>(window->Width());
		m_WinHeight = static_cast<float>(window->Height());


		//glEnable(GL_BLEND);
		//glBlendEquation(GL_FUNC_ADD);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LESS);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT); // Or GL_FRONT, depending on your winding order

		m_Batchers.emplace("Cube", std::make_unique<CubeBatchRenderer<DrawInstance, CubeInstanceMesh>>(10000));
		m_Batchers.emplace("Line", std::make_unique<LineBatchRenderer<DrawInstance, LineInstanceMesh>>(5000));
		m_Batchers.emplace("Triangle", std::make_unique<TriangleBatchRenderer<DrawInstance, TriangleInstanceMesh>>(10000));
		m_Batchers.emplace("Circle", std::make_unique<CircleBatchRenderer<DrawInstance, CircleInstanceMesh>>(10000));
		m_Batchers.emplace("Quad", std::make_unique<QuadBatchRenderer<DrawInstance, QuadInstanceMesh>>(10000));


		m_RenderCommands.resize(static_cast<int>(RenderCommandTypes::QUEUES));

		m_RenderCommands[static_cast<int>(RenderCommandTypes::Opaque)] = std::make_unique<RenderQueue<ElementData, ElementDepthCompare>>();
		m_RenderCommands[static_cast<int>(RenderCommandTypes::Transparent)] = std::make_unique<RenderQueue<ElementData, ElementDepthCompare>>();
		m_RenderCommands[static_cast<int>(RenderCommandTypes::Emissive)] = std::make_unique<RenderQueue<ElementData, ElementDepthCompare>>();
		m_RenderCommands[static_cast<int>(RenderCommandTypes::Particles)] = std::make_unique<RenderQueue<ParticleData, ParticleDepthCompare>>();
		m_RenderCommands[static_cast<int>(RenderCommandTypes::UI)] = std::make_unique<RenderQueue<ElementData, ElementDepthCompare>>();
		m_RenderCommands[static_cast<int>(RenderCommandTypes::Debug)] = std::make_unique<RenderQueue<ElementData, ElementDepthCompare>>();


		//m_Framebuffers.insert(std::make_pair("Scene", Framebuffer(static_cast<unsigned int>(m_WinWidth), static_cast<unsigned int>(m_WinHeight))));

		//Framebuffer& buffer = m_Framebuffers.at("Scene");

		//buffer.Init();

		//buffer.AddColorAttachment({ Framebuffer::Format(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE), Framebuffer::Format(GL_R32I, GL_RED_INTEGER, GL_INT) });


		//for (auto& batch : m_Batchers)
		//	batch.second.get()->BindTextureBuffer();
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
		for (const auto& pass : m_RenderPasses)
		{
			// Bind framebuffer, clear etc
			if (pass.framebuffer)
			{
				pass.framebuffer->Bind();

				//int nean = -1;
				//pass.framebuffer->ClearColorAttachment(1, &nean);

				if (pass.clear) 				
					ClearRender_Params(pass.clearColor.r, pass.clearColor.g, pass.clearColor.b, pass.clearColor.a);

			}
			else
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				ClearRender();
			}


			SetRenderState(pass);
			if (pass.stage != RenderStage::Final)
			{
				for (auto& batch : m_Batchers)
					batch.second->BeginBatch();
			}

			switch (pass.stage)
			{
			case RenderStage::Opaque:
				DrawQueue(RenderCommandTypes::Opaque, pass);
				Flush(pass);
				//return;
				break;

			case RenderStage::Transparent:
				DrawQueue(RenderCommandTypes::Transparent, pass);
				Flush(pass);

				break;

			case RenderStage::Particles:
				DrawQueue(RenderCommandTypes::Particles, pass);
				Flush(pass);
				break;


			case RenderStage::Emissive:
				DrawQueue(RenderCommandTypes::Emissive, pass);
				Flush(pass);
				break;

			case RenderStage::UI:
				DrawQueue(RenderCommandTypes::UI, pass);
				Flush(pass);

				break;

			case RenderStage::Debug:
				DrawQueue(RenderCommandTypes::Debug, pass);
				Flush(pass);

				break;
			case RenderStage::PostProcess:
			case RenderStage::Final: 
				RenderCompositePass(pass);
				break;

			default:
				break;
			}


			if (pass.framebuffer)
				pass.framebuffer->Unbind();
			else
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}


	void NRenderer::DrawQueue(RenderCommandTypes type, const RenderPass& pass)
	{
		auto& queue = m_RenderCommands[static_cast<int>(type)];
		if (!queue || queue->Empty())
			return;

		while (!queue->Empty())
		{
			auto renderData = queue->Pop();

			// Render depending on type
			if (type == RenderCommandTypes::Particles)
			{
				RenderParticles(static_cast<ParticleData*>(renderData.get()));
			}
			else
			{
				RenderInstances(static_cast<ElementData*>(renderData.get()), pass.shader);
			}
		}
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
		m_RenderQueue.push(std::move(render));
	}

	void NRenderer::AddDebugRenderCall(std::unique_ptr<ElementData>&& render)
	{
		m_DebugRenderQueue.push(std::move(render));
	}

	void NRenderer::AddParticleRenderCall(std::unique_ptr<ParticleData>&& render)
	{
		m_ParticleRenderQueue.push(std::move(render));
	}

	void NRenderer::AddEmissiveRenderCall(std::unique_ptr<ElementData>&& render)
	{
		m_EmissiveRenderQueue.push(std::move(render));
	}


	void NRenderer::AddRenderCall(RenderCommandTypes type, std::unique_ptr<RenderData>&& data)
	{
		auto& queue = m_RenderCommands[static_cast<int>(type)];
		if (queue)
		{
			// We need to cast to the concrete RenderQueue type for Push
			if (type == RenderCommandTypes::Particles)
			{
				auto particleQueue = dynamic_cast<RenderQueue<ParticleData, ParticleDepthCompare>*>(queue.get());
				if (particleQueue)
					particleQueue->Push(std::unique_ptr<ParticleData>(static_cast<ParticleData*>(data.release())));
			}
			else
			{
				auto elementQueue = dynamic_cast<RenderQueue<ElementData, ElementDepthCompare>*>(queue.get());
				if (elementQueue)
					elementQueue->Push(std::unique_ptr<ElementData>(static_cast<ElementData*>(data.release())));
			}
		}
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

	void NRenderer::ClearRender()
	{
		glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void NRenderer::ClearRender_Params(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void NRenderer::ClearRenderS()
	{
		glClearColor(.1f, .1f, .1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	bool NRenderer::OnWindowResize(const WindowResizeEvent& e)
	{
		if (!m_Parent->GetIsEditorEnabled())
		{
			m_WinWidth = static_cast<float>(e.GetWidth());
			m_WinHeight = static_cast<float>(e.GetHeight());

			for (auto& fb : m_Framebuffers)
			{
				fb.second.Resize(static_cast<unsigned int>(m_WinWidth), static_cast<unsigned int>(m_WinHeight));
			}

			SetViewport(0, 0, static_cast<uint32_t>(m_WinWidth), static_cast<uint32_t>(m_WinHeight));
		}

		return true;
	}
}