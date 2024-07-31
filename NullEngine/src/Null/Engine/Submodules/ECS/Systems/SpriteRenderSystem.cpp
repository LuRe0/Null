
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

		componentFactory->Register<SpriteComponent>(CreateSpriteComponent,
			[this](Entity& id) { this->ViewSpriteComponent(id); }, WriteSpriteComponent);
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

			if (!sprite.m_Enabled)
				continue;

			//model, mesh, spritesrc, tint, shadername, frameindex, entity
			renderer->AddRenderCall({ transform.m_TransformMatrix, sprite.m_Mesh,sprite.m_SpriteSource, sprite.m_Color, sprite.m_ShaderName, sprite.m_FrameIndex, entityId });
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
		JsonReader jsonWrapper(json);


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

	JSON SpriteRenderSystem::WriteSpriteComponent(BaseComponent* component)
	{
		nlohmann::json json;

		auto& sprite = *static_cast<SpriteComponent*>(component);

		json["Sprite"]["frameindex"] = sprite.m_FrameIndex;
		json["Sprite"]["texture"] = sprite.m_SpriteSource->GetName();
		json["Sprite"]["dimension"] = { sprite.m_SpriteSource->GetRows(),  sprite.m_SpriteSource->GetCols()};
		json["Sprite"]["tint"] = { sprite.m_Color.r, sprite.m_Color.g, sprite.m_Color.b, sprite.m_Color.a };
		json["Sprite"]["shadername"] = sprite.m_ShaderName;
		json["Sprite"]["meshname"] = sprite.m_Mesh->GetName();

		return json;

	}

	void SpriteRenderSystem::ViewSpriteComponent(Entity& entity)
	{
		SpriteComponent& sprite = entity.Get<SpriteComponent>();
		NSpriteSourceManager* spritesrcManager = NEngine::Instance().Get<NSpriteSourceManager>();
		NTextureManager* texureManager = NEngine::Instance().Get<NTextureManager>();
		NMeshManager* meshManager = NEngine::Instance().Get<NMeshManager>();
		NShaderManager* shaderManager = NEngine::Instance().Get<NShaderManager>();


		ImGui::DragInt("Frame Index", reinterpret_cast<int*>(&(sprite.m_FrameIndex)), 0.5f, 0);
		const auto& meshNames = meshManager->GetResourceNames();




		if (sprite.m_Mesh)
		{
			if (ImGui::BeginCombo("Select Mesh", sprite.m_Mesh->GetName().c_str()))
			{
				for (const auto& name : meshNames)
				{
					bool isSelected = sprite.m_Mesh->GetName() == name;

					if (ImGui::Selectable(name.c_str(), isSelected)) {
						sprite.m_Mesh = meshManager->Get(name);
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus(); // Set focus on the selected item
					}
				}
				ImGui::EndCombo();
			}
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			if (ImGui::BeginCombo("Select Mesh", "No Mesh Selected"))
			{
				ImGui::PopStyleColor();

				for (const auto& name : meshNames)
				{
					bool isSelected = false;

					if (ImGui::Selectable(name.c_str(), isSelected)) {
						sprite.m_Mesh = meshManager->Get(name);
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus(); // Set focus on the selected item
					}
				}
				ImGui::EndCombo();
			}
			else
			{
				ImGui::PopStyleColor();
			}


		}
		if (sprite.m_SpriteSource)
		{
			ImGui::DragInt("Rows", &sprite.m_SpriteSource->Rows(), 0.5f, 1);
			ImGui::DragInt("Columns", &sprite.m_SpriteSource->Cols(), 0.5f, 1);

			if (sprite.m_SpriteSource->GetTexture())
			{
				ImGui::Text("Texture\t"); ImGui::Image((void*)(__int64)sprite.m_SpriteSource->GetTexture()->GetID(), ImVec2(100, 100), { 0, -1 }, { 1, 0 }, ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
				ImGui::SetCursorPos({ ImGui::GetCursorPos().x, ImGui::GetCursorPos().y - 100 }); // Move the cursor back to the position of the image
				if (ImGui::InvisibleButton("ImageButton", ImVec2(125, 100)))
				{
					ImGui::OpenPopup("TexturePopup");
				}
			}

		}
		else
		{
			if (ImGui::Button("Select texture", ImVec2(125, 100)))
				ImGui::OpenPopup("TexturePopup");
		}
		if (ImGui::BeginPopup("TexturePopup"))
		{
			ImGui::SetNextWindowSize(ImVec2(125, 100), ImGuiCond_FirstUseEver);
			// Begin a child window to make it scrollable
			ImGui::BeginChild("TextureList", ImVec2(125, 200), true, ImGuiWindowFlags_AlwaysUseWindowPadding);

			const auto& componentsNames = texureManager->GetResourceNames();

			for (const auto& name : componentsNames)
			{
				auto texture = texureManager->Get(name);
				if (texture)
				{
					ImGui::Text("%s :", name.c_str());
					if (ImGui::ImageButton((void*)(__int64)texture->GetID(), ImVec2(75, 50), { 0, -1 }, { 1, 0 }))
					{
						sprite.m_SpriteSource = spritesrcManager->Has(name) ? spritesrcManager->Get(name) : spritesrcManager->Create(name,1,1);
						ImGui::CloseCurrentPopup();
					}
				}
			}
			ImGui::EndChild(); // End the child window
			ImGui::EndPopup();
		}

		ImGui::ColorEdit4("Tint", glm::value_ptr(sprite.m_Color), 0.5f);

		const auto& shaderNames = shaderManager->GetResourceNames();

		if (ImGui::BeginCombo("Select Shader", sprite.m_ShaderName.c_str()))
		{
			for (const auto& name : shaderNames)
			{
				bool isSelected = sprite.m_ShaderName == name;

				if (ImGui::Selectable(name.c_str(), isSelected)) {
					sprite.m_ShaderName = name;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus(); // Set focus on the selected item
				}
			}

			ImGui::EndCombo();
		}

		if (!entity.Has<TransformComponent>())
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Requires a Transform component");

		//ImGui::DragFloat3("Rotation", glm::value_ptr(transform.m_Rotation), 0.5f);
		//ImGui::DragFloat3("Scale", glm::value_ptr(transform.m_Scale), 0.5f);

		//unsigned int m_FrameIndex;
		//SpriteSource* m_SpriteSource;
		//Mesh* m_Mesh;
		//glm::vec4 m_Color;
		//std::string m_ShaderName;

	}
}