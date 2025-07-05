
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
#include "Null/Engine/Submodules/Graphics/Mesh/Mesh.h"
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
		NCameraManager* camManager = NEngine::Instance().Get<NCameraManager>();

		glm::mat4 viewMatrix = camManager->GetCurrentCamera()->GetViewMatrix();


		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = m_Parent->GetComponent<TransformComponent>(entityId);
			SpriteComponent& sprite = m_Parent->GetComponent<SpriteComponent>(entityId);

			if (!sprite.m_Enabled)
				continue;

			//if (!camManager->IsWithinFrustum(transform.m_Translation, (transform.m_Scale/2.0f)))
			//	continue;

			glm::vec4 worldPosition = transform.m_TransformMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

			// Transform the world position to camera space
			glm::vec4 cameraSpacePosition = viewMatrix * worldPosition;

			// The depth is the z-component of the camera space position
			float depth = cameraSpacePosition.z;

			if (sprite.m_Color.a < 1.0f)
			{
				renderer->AddRenderCall(RenderCommandTypes::Transparent, std::make_unique<ElementData>(transform.m_TransformMatrix, sprite.m_Mesh, sprite.m_SpriteSource, sprite.m_Color, sprite.m_ShaderName,
					sprite.m_FrameIndex, entityId, sprite.m_Thickness, sprite.m_Fade, RenderData::INSTANCED, -depth));
			}
			else
			{
				renderer->AddRenderCall(RenderCommandTypes::Opaque, std::make_unique<ElementData>(transform.m_TransformMatrix, sprite.m_Mesh, sprite.m_SpriteSource, sprite.m_Color,  sprite.m_ShaderName,
					sprite.m_FrameIndex, entityId, sprite.m_Thickness, sprite.m_Fade, RenderData::INSTANCED, -depth));
			}


			const SpriteSource* emissiveSource = sprite.m_EmissiveSpriteSource ? sprite.m_EmissiveSpriteSource : sprite.m_SpriteSource;
			if (sprite.m_EmissiveStrength > 0.0f) 
			{
				glm::mat4 emissiveTransform = transform.m_TransformMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.01f));
				glm::vec4 emissiveTint = sprite.m_EmissiveColor * sprite.m_EmissiveStrength;
				renderer->AddRenderCall(RenderCommandTypes::Emissive, std::make_unique<ElementData>(
					emissiveTransform,
					sprite.m_Mesh,
					sprite.m_EmissiveSpriteSource ? sprite.m_EmissiveSpriteSource : sprite.m_SpriteSource,
					emissiveTint,
					sprite.m_ShaderName,
					sprite.m_FrameIndex,
					entityId,
					sprite.m_Thickness,
					sprite.m_Fade,
					RenderData::INSTANCED,
					-depth));
			}


			//model, mesh, spritesrc, tint, shadername, frameindex, entity
	
		}
	}

	void SpriteRenderSystem::Unload()
	{
	}

	void SpriteRenderSystem::Shutdown()
	{
	}

	void SpriteRenderSystem::RegisterToScripAPI(sol::state& lua)
	{
		lua.new_usertype<SpriteComponent>
			(
				"Sprite",
				sol::no_constructor,
				"type_id", &Component<SpriteComponent>::GetID,
				"frame_index", sol::readonly(&SpriteComponent::m_FrameIndex),
				"tint", sol::readonly(&SpriteComponent::m_Color),
				"get_alpha", sol::overload(
					[](SpriteComponent& sprite, float a)
					{
						return sprite.m_Color.a;
					}
				),
				"set_alpha", sol::overload(
					[](SpriteComponent& sprite, float a)
					{
						sprite.m_Color = glm::vec4(sprite.m_Color.r, sprite.m_Color.g, sprite.m_Color.b, a);
					}
				),
				"set_tint", sol::overload(
					[](SpriteComponent& sprite, float r, float g, float b, float a)
					{
						sprite.m_Color = glm::vec4(r, g, b, a);
					},
					[](SpriteComponent& sprite, const glm::vec4& newColor)
					{
						sprite.m_Color = newColor;
					}
				),
				"set_frame_index", [](SpriteComponent& sprite, int i)
				{
					sprite.m_FrameIndex = i;
				}
		);
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
			comp->m_Fade = jsonWrapper.GetFloat("fade", 0.005f);
			comp->m_Thickness = std::clamp(comp->m_Thickness, 0.02f, jsonWrapper.GetFloat("thickness", 1.0f));

			comp->m_FrameIndex = jsonWrapper.GetInt("frameindex", 0);
			comp->m_EmissiveStrength = jsonWrapper.GetFloat("emissiveStrength", 0);
			glm::vec2 dimension = jsonWrapper.GetVec2("dimension", { 1.0f, 1.0f });
			auto src = jsonWrapper.GetString("texture", "");
			if (!src.empty())
				comp->m_SpriteSource = spritesrcManager->Create(src, static_cast<int>(dimension.x), static_cast<int>(dimension.y));
			else
				comp->m_SpriteSource = nullptr;


			src = jsonWrapper.GetString("emissiveTexture", "");
			if (!src.empty())
				comp->m_EmissiveSpriteSource = spritesrcManager->Create(src, static_cast<int>(dimension.x), static_cast<int>(dimension.y));
			else
				comp->m_EmissiveSpriteSource = nullptr;

			comp->m_ShaderName = jsonWrapper.GetString("shadername", "default");
			const std::string& meshName = jsonWrapper.GetString("meshname", "");

			//if no path for mesh file, defaults to create a quad
			comp->m_Mesh = !meshName.empty() ? meshManager->Create(meshName) : meshManager->Create("Quad");


			comp->m_Color = jsonWrapper.GetVec4("tint", { 1.0f, 1.0f, 1.0f, 1.0f });
			comp->m_EmissiveColor = jsonWrapper.GetVec4("emissiveColor", { 1.0f, 1.0f, 1.0f, 1.0f });
		}

		componentFactory->AddOrUpdate<SpriteComponent>(id, comp, registry, comp->m_FrameIndex, comp->m_SpriteSource, comp->m_EmissiveSpriteSource, comp->m_Mesh,
			comp->m_Color, comp->m_EmissiveColor, comp->m_EmissiveStrength, comp->m_ShaderName, comp->m_Thickness, comp->m_Fade);
	}

	JSON SpriteRenderSystem::WriteSpriteComponent(BaseComponent* component)
	{
		nlohmann::json json;

		auto& sprite = *static_cast<SpriteComponent*>(component);

		json["Sprite"]["frameindex"] = sprite.m_FrameIndex;
		json["Sprite"]["texture"] = sprite.m_SpriteSource != nullptr ? sprite.m_SpriteSource->GetName() : "";
		json["Sprite"]["emissiveTexture"] = sprite.m_EmissiveSpriteSource != nullptr ? sprite.m_EmissiveSpriteSource->GetName() : "";
		json["Sprite"]["dimension"] = sprite.m_SpriteSource == nullptr
			? nlohmann::json::array({ 1, 1 })
			: nlohmann::json::array({ sprite.m_SpriteSource->GetRows(), sprite.m_SpriteSource->GetCols() });
		json["Sprite"]["tint"] = { sprite.m_Color.r, sprite.m_Color.g, sprite.m_Color.b, sprite.m_Color.a };
		json["Sprite"]["emissiveColor"] = { sprite.m_EmissiveColor.r, sprite.m_EmissiveColor.g, sprite.m_EmissiveColor.b, sprite.m_EmissiveColor.a };
		json["Sprite"]["shadername"] = sprite.m_ShaderName;
		json["Sprite"]["meshname"] = sprite.m_Mesh == nullptr ? "" : sprite.m_Mesh->GetName();
		json["Sprite"]["fade"] = sprite.m_Fade;
		json["Sprite"]["emissiveStrength"] = sprite.m_EmissiveStrength;
		json["Sprite"]["thickness"] = sprite.m_Thickness;

		return json;

	}

	void SpriteRenderSystem::ViewSpriteComponent(Entity& entity)
	{
		SpriteComponent& sprite = entity.Get<SpriteComponent>();
		NSpriteSourceManager* spritesrcManager = NEngine::Instance().Get<NSpriteSourceManager>();
		NTextureManager* texureManager = NEngine::Instance().Get<NTextureManager>();
		NMeshManager* meshManager = NEngine::Instance().Get<NMeshManager>();
		NShaderManager* shaderManager = NEngine::Instance().Get<NShaderManager>();



		ImGui::DragFloat("Thickness", &sprite.m_Thickness, 0.02f, 0.01f, 1.0f);

		ImGui::DragFloat("Fade", &sprite.m_Fade, 0.001f, 0);

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

		DrawDragDrop("Main", sprite, sprite.m_SpriteSource, texureManager, spritesrcManager);
		DrawDragDrop("Emissive", sprite, sprite.m_EmissiveSpriteSource, texureManager, spritesrcManager);


		ImGui::Text("Source Dimensions");
		if (sprite.m_SpriteSource)
			if (ImGui::DragInt(std::string(std::string("Rows")).c_str(), &sprite.m_SpriteSource->Rows(), 0.5f, 1))
				if (sprite.m_EmissiveSpriteSource)
					sprite.m_EmissiveSpriteSource->Rows() = sprite.m_SpriteSource->GetRows();
		if (sprite.m_SpriteSource)
			if(ImGui::DragInt(std::string(std::string("Cols")).c_str(), &sprite.m_SpriteSource->Cols(), 0.5f, 1))
				if (sprite.m_EmissiveSpriteSource)
					sprite.m_EmissiveSpriteSource->Cols() = sprite.m_SpriteSource->GetCols();

		if (sprite.m_SpriteSource)
			ImGui::DragInt(std::string(std::string("Frame")).c_str(), reinterpret_cast<int*>(&sprite.m_FrameIndex), 1, 0, sprite.m_SpriteSource->GetFrameCount());

		ImGui::ColorEdit4("Tint", glm::value_ptr(sprite.m_Color));
		ImGui::ColorEdit4("Emissive Color", glm::value_ptr(sprite.m_EmissiveColor));


		ImGui::DragFloat("Emissive Strength", &sprite.m_EmissiveStrength, 0.001f, 0);

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

	void SpriteRenderSystem::DrawDragDrop(const char* label, SpriteComponent& sprite, SpriteSource*& source, NTextureManager* texMgr, NSpriteSourceManager* srcMgr)
	{
		if (source)
		{
			if (source->GetTexture())
			{
				ImGui::Text("Texture\t");
				ImGui::Image((void*)(intptr_t)source->GetTexture()->GetID(), ImVec2(125, 100), { 0, -1 }, { 1, 0 });

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 100);
				std::string btnId = std::string("##ImageButton_") + label;
				if (ImGui::InvisibleButton(btnId.c_str(), ImVec2(125, 100)))
				{
					ImGui::OpenPopup((std::string("TexturePopup_") + label).c_str());
				}
			}
		}
		else
		{
			if (ImGui::Button((std::string("Select ") + label).c_str(), ImVec2(125, 100)))
			{
				ImGui::OpenPopup((std::string("TexturePopup_") + label).c_str());
			}
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_FILE"))
			{
				std::string filename((const char*)payload->Data);
				if (!filename.empty())
					source = srcMgr->Has(filename) ? srcMgr->Get(filename) : srcMgr->Create(filename, 1, 1);
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginPopup((std::string("TexturePopup_") + label).c_str()))
		{
			ImGui::SetNextWindowSize(ImVec2(125, 100), ImGuiCond_FirstUseEver);
			ImGui::BeginChild((std::string("TextureList_") + label).c_str(), ImVec2(125, 200), true);

			if (ImGui::Selectable("⨯ None"))
			{
				source = nullptr;
				ImGui::CloseCurrentPopup();
			}

			for (const auto& name : texMgr->GetResourceNames())
			{
				auto texture = texMgr->Get(name);
				if (texture)
				{
					ImGui::Text("%s :", name.c_str());
					if (ImGui::ImageButton((void*)(intptr_t)texture->GetID(), ImVec2(75, 50), { 0, -1 }, { 1, 0 }))
					{
						source = srcMgr->Has(name) ? srcMgr->Get(name) : srcMgr->Create(name, 1, 1);
						ImGui::CloseCurrentPopup();
					}
				}
			}

			ImGui::EndChild();
			ImGui::EndPopup();
		}
	}

}