
//------------------------------------------------------------------------------
//
// File Name:	SpriteRenderSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "SpriteRenderSystem.h"
#include "Null/Engine/Submodules/Graphics/Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

const glm::vec2 GRAVITY(0.0f, -9.81f);

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	SpriteRenderSystem::SpriteRenderSystem()
	{
		Require<TransformComponent>();
		Require<SpriteComponent>();

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		componentFactory->Register<SpriteComponent>(CreateSpriteComponent, [this](Entity& id) { this->ViewSpriteComponent(id); });
	}

	void SpriteRenderSystem::Load()
	{
	}

	void SpriteRenderSystem::Init()
	{
		ISystem::Init();

	}

	void SpriteRenderSystem::Update(float dt)
	{

	}

	void SpriteRenderSystem::Render()
	{
		NRenderer* renderer = NEngine::Instance().Get<NRenderer>();
		NRegistry* m_Parent = NEngine::Instance().Get<NRegistry>();

		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = m_Parent->GetComponent<TransformComponent>(entityId);
			SpriteComponent& sprite = m_Parent->GetComponent<SpriteComponent>(entityId);

			//model, mesh, spritesrc, tint, shadername, frameindex
			renderer->AddRenderCall({ transform.m_TransformMatrix, sprite.m_Mesh,sprite.m_SpriteSource, sprite.m_Color, sprite.m_ShaderName, sprite.m_FrameIndex });
		}
	}

	void SpriteRenderSystem::Unload()
	{
	}

	void SpriteRenderSystem::Shutdown()
	{
	}


	void SpriteRenderSystem::CreateSpriteComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{
		NSpriteSourceManager* spritesrcManager = NEngine::Instance().Get<NSpriteSourceManager>();
		NMeshManager* meshManager = NEngine::Instance().Get<NMeshManager>();
		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		auto* comp = static_cast<SpriteComponent*>(component);
		JsonWrapper jsonWrapper(json);


		if (!jsonWrapper.Empty())
		{
			comp->m_FrameIndex = jsonWrapper.GetInt("frameindex", 0);
			glm::vec2 dimension = jsonWrapper.GetVec2("dimension", { 1.0f, 1.0f });
			comp->m_SpriteSource = spritesrcManager->Create(jsonWrapper.GetString("texture", ""), dimension.x, dimension.y);
			comp->m_ShaderName = jsonWrapper.GetString("shadername", "default");
			const std::string& meshName = jsonWrapper.GetString("meshname", "");

			//if no path for mesh file, defaults to create a quad
			comp->m_Mesh = !meshName.empty() ? meshManager->Create(meshName) : meshManager->Create("Quad");


			comp->m_Color = jsonWrapper.GetVec4("tint", { 1.0f, 1.0f, 1.0f, 1.0f });
		}

		componentFactory->AddOrUpdate<SpriteComponent>(id, comp, registry, comp->m_FrameIndex, comp->m_SpriteSource, comp->m_Mesh, comp->m_Color, comp->m_ShaderName);
	}

	void SpriteRenderSystem::ViewSpriteComponent(Entity& entity)
	{
		SpriteComponent& sprite = entity.Get<SpriteComponent>();

		ImGui::DragInt("Frame Index", reinterpret_cast<int*>(&(sprite.m_FrameIndex)), 0.5f, 0);

		ImGui::DragInt("Rows", &sprite.m_SpriteSource->Rows(), 0.5f, 0);
		ImGui::DragInt("Columns", &sprite.m_SpriteSource->Cols(), 0.5f, 0);
		ImGui::DragFloat4("Tint", glm::value_ptr(sprite.m_Color), 0.5f);
		ImGui::InputText("##Shader Name", &sprite.m_ShaderName);
	
		if (sprite.m_SpriteSource->GetTexture())
		{
			ImGui::Text("Texture\t"); ImGui::Image((void*)(__int64)sprite.m_SpriteSource->GetTexture()->GetID(), ImVec2(100, 100), {0, -1}, {1, 0}, ImVec4(1,1,1,1), ImVec4(1, 1, 1, 1));
		}
		else
		{
			ImGui::Button("Drop texture", ImVec2(100, 100));
		}


		//ImGui::DragFloat3("Rotation", glm::value_ptr(transform.m_Rotation), 0.5f);
		//ImGui::DragFloat3("Scale", glm::value_ptr(transform.m_Scale), 0.5f);

		//unsigned int m_FrameIndex;
		//SpriteSource* m_SpriteSource;
		//Mesh* m_Mesh;
		//glm::vec4 m_Color;
		//std::string m_ShaderName;

	}
}