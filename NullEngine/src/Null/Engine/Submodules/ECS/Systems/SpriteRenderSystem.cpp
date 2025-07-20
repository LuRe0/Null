
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
#include "../Entities/Entity.h"	
#include "../../../../Tools/ImGuiH.h"

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

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register<SpriteComponent>(CreateSpriteComponent,
			[this](Entity& id) { this->ViewSpriteComponent(id); }, WriteSpriteComponent,
			AddSpriteComponent, DiffSpriteComponent);
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
		NRenderer* renderer = NRenderer::Instance();
		NRegistry* m_Parent = NRegistry::Instance();
		NCameraManager* camManager = NCameraManager::Instance();

		glm::mat4 viewMatrix = camManager->GetCurrentCamera()->GetViewMatrix();


		for (const auto entityId : GetSystemEntities())
		{
			TransformComponent& transform = m_Parent->GetComponent<TransformComponent>(entityId);
			SpriteComponent& sprite = m_Parent->GetComponent<SpriteComponent>(entityId);

			if (!sprite.m_ComponentFlags.IsSet(ComponentFlags_Enabled))
				continue;

			//if (!camManager->IsWithinFrustum(transform.m_Translation, (transform.m_Scale/2.0f)))
			//	continue;

			glm::vec4 worldPosition = transform.transformMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

			// Transform the world position to camera space
			glm::vec4 cameraSpacePosition = viewMatrix * worldPosition;

			// The depth is the z-component of the camera space position
			float depth = cameraSpacePosition.z;


			Mesh* mesh = sprite.m_MeshNameID ? NMeshManager::Instance()->Get(sprite.m_MeshNameID) : nullptr;
			SpriteSource* src = sprite.m_SpriteSourceNameID ? NSpriteSourceManager::Instance()->Get(sprite.m_SpriteSourceNameID)  : nullptr;
			SpriteSource* emissiveSrc = sprite.m_EmissiveSpriteSourceNameID ? NSpriteSourceManager::Instance()->Get(sprite.m_EmissiveSpriteSourceNameID) : nullptr;

			if (sprite.m_Color.a < 1.0f)
			{
				renderer->AddRenderCall(RenderCommandTypes::Transparent, std::make_unique<ElementData>(transform.transformMatrix, mesh, src, sprite.m_Color, "",
					sprite.m_FrameIndex, entityId, sprite.m_Thickness, sprite.m_Fade, RenderData::INSTANCED, -depth));
			}
			else
			{
				renderer->AddRenderCall(RenderCommandTypes::Opaque, std::make_unique<ElementData>(transform.transformMatrix, mesh, src, sprite.m_Color, "",
					sprite.m_FrameIndex, entityId, sprite.m_Thickness, sprite.m_Fade, RenderData::INSTANCED, -depth));
			}


			SpriteSource* selectedSrc = emissiveSrc != nullptr ? emissiveSrc : src;
			if (sprite.m_EmissiveStrength > 0.0f)
			{
				glm::mat4 emissiveTransform = transform.transformMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.01f));
				glm::vec4 emissiveTint = sprite.m_EmissiveColor * sprite.m_EmissiveStrength;
				renderer->AddRenderCall(RenderCommandTypes::Emissive, std::make_unique<ElementData>(
					emissiveTransform,
					mesh,
					selectedSrc,
					emissiveTint,
					"",
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
				"emissive_color", sol::readonly(&SpriteComponent::m_EmissiveColor),
				"emissive_strength", sol::readonly(&SpriteComponent::m_EmissiveStrength),
				"get_alpha", sol::overload(
					[](SpriteComponent& sprite)
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
				"set_emissive_color", sol::overload(
					[](SpriteComponent& sprite, float r, float g, float b, float a)
					{
						sprite.m_EmissiveColor = glm::vec4(r, g, b, a);
					},
					[](SpriteComponent& sprite, const glm::vec4& newColor)
					{
						sprite.m_EmissiveColor = newColor;
					}
				),
				"set_emissive_strength", sol::overload(
					[](SpriteComponent& sprite, float s)
					{
						sprite.m_EmissiveStrength = s;
					}
				),
				"set_frame_index", [](SpriteComponent& sprite, int i)
				{
					sprite.m_FrameIndex = i;
				}
			);
	}


	void SpriteRenderSystem::CreateSpriteComponent(void* component, const nlohmann::json& json)
	{
		NSpriteSourceManager* spritesrcManager = NSpriteSourceManager::Instance();
		NMeshManager* meshManager = NMeshManager::Instance();
		NComponentFactory* componentFactory = NComponentFactory::Instance();

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
			{
				comp->m_SpriteSourceNameID = STRID(src);
				spritesrcManager->Create(src, static_cast<int>(dimension.x), static_cast<int>(dimension.y));
			}

			src = jsonWrapper.GetString("emissiveTexture", "");
			if (!src.empty())
			{
				comp->m_EmissiveSpriteSourceNameID = STRID(src);
				spritesrcManager->Create(src, static_cast<int>(dimension.x), static_cast<int>(dimension.y));
			}



			const std::string& meshName = jsonWrapper.GetString("meshname", "");

			comp->m_MeshNameID = !meshName.empty() ? STRID(meshName) : STRID("Quad");


			comp->m_Color = jsonWrapper.GetVec4("tint", { 1.0f, 1.0f, 1.0f, 1.0f });
			comp->m_EmissiveColor = jsonWrapper.GetVec4("emissiveColor", { 1.0f, 1.0f, 1.0f, 1.0f });


			ComponentFlagSet flags;
			flags.Set(ComponentFlags_Enabled);
			flags.Set(ComponentFlags_Serialized);
			comp->m_ComponentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);
		}

	}

	void SpriteRenderSystem::AddSpriteComponent(void* component, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<SpriteComponent*>(component);


		componentFactory->AddOrUpdate<SpriteComponent>(id, comp, registry, comp->m_Color, comp->m_EmissiveColor, comp->m_FrameIndex, comp->m_SpriteSourceNameID,
			comp->m_EmissiveSpriteSourceNameID, comp->m_MeshNameID, comp->m_EmissiveStrength, comp->m_Thickness, comp->m_Fade, comp->m_ComponentFlags);

	}

	JSON SpriteRenderSystem::WriteSpriteComponent(const void* component)
	{
		nlohmann::json json;

		auto& sprite = *static_cast<const SpriteComponent*>(component);

		json["Sprite"]["frameindex"] = sprite.m_FrameIndex;
		json["Sprite"]["texture"] = STRFROM(sprite.m_SpriteSourceNameID);
		json["Sprite"]["emissiveTexture"] = STRFROM(sprite.m_EmissiveSpriteSourceNameID);
		json["Sprite"]["dimension"] = sprite.m_SpriteSourceNameID
			? nlohmann::json::array({ 1, 1 })
			: nlohmann::json::array({ NSpriteSourceManager::Instance()->Get(sprite.m_SpriteSourceNameID)->GetRows(), NSpriteSourceManager::Instance()->Get(sprite.m_SpriteSourceNameID)->GetCols() });
		json["Sprite"]["tint"] = { sprite.m_Color.r, sprite.m_Color.g, sprite.m_Color.b, sprite.m_Color.a };
		json["Sprite"]["emissiveColor"] = { sprite.m_EmissiveColor.r, sprite.m_EmissiveColor.g, sprite.m_EmissiveColor.b, sprite.m_EmissiveColor.a };
		json["Sprite"]["meshname"] = STRFROM(sprite.m_MeshNameID);
		json["Sprite"]["fade"] = sprite.m_Fade;
		json["Sprite"]["emissiveStrength"] = sprite.m_EmissiveStrength;
		json["Sprite"]["thickness"] = sprite.m_Thickness;
		json["Sprite"]["ComponentFlags"] = sprite.m_ComponentFlags.m_Flags;

		return json;

	}

	JSON SpriteRenderSystem::DiffSpriteComponent(const void* base, const void* modified)
	{
		JSON diff;

		auto& b = *static_cast<const SpriteComponent*>(modified);
		auto& a = *static_cast<const SpriteComponent*>(base);

		if (a.m_FrameIndex != b.m_FrameIndex)
			diff["frameindex"] = b.m_FrameIndex;

		const std::string texA = a.m_SpriteSourceNameID ? STRFROM(a.m_SpriteSourceNameID) : "";
		const std::string texB = b.m_SpriteSourceNameID ? STRFROM(b.m_SpriteSourceNameID) : "";
		if (texA != texB)
			diff["texture"] = texB;

		const std::string emisTexA = a.m_EmissiveSpriteSourceNameID ? STRFROM(a.m_EmissiveSpriteSourceNameID) : "";
		const std::string emisTexB = b.m_EmissiveSpriteSourceNameID ? STRFROM(b.m_EmissiveSpriteSourceNameID) : "";
		if (emisTexA != emisTexB)
			diff["emissiveTexture"] = emisTexB;

		if (a.m_SpriteSourceNameID && b.m_SpriteSourceNameID)
		{
			auto* srcA = NSpriteSourceManager::Instance()->Get(a.m_SpriteSourceNameID);
			auto* srcB = NSpriteSourceManager::Instance()->Get(b.m_SpriteSourceNameID);

			if (srcA->GetRows() != srcB->GetRows() ||
				srcA->GetCols() != srcB->GetCols())
			{
				diff["dimension"] = { srcB->GetRows(), srcB->GetCols() };
			}
		}

		if (a.m_Color != b.m_Color)
			diff["tint"] = { b.m_Color.r, b.m_Color.g, b.m_Color.b, b.m_Color.a };

		if (a.m_EmissiveColor != b.m_EmissiveColor)
			diff["emissiveColor"] = { b.m_EmissiveColor.r, b.m_EmissiveColor.g, b.m_EmissiveColor.b, b.m_EmissiveColor.a };


		const std::string meshA = a.m_MeshNameID ? STRFROM(a.m_MeshNameID) : "";
		const std::string meshB = b.m_MeshNameID ? STRFROM(a.m_MeshNameID) : "";
		if (meshA != meshB)
			diff["meshname"] = meshB;

		if (a.m_Fade != b.m_Fade)
			diff["fade"] = b.m_Fade;

		if (a.m_EmissiveStrength != b.m_EmissiveStrength)
			diff["emissiveStrength"] = b.m_EmissiveStrength;

		if (a.m_Thickness != b.m_Thickness)
			diff["thickness"] = b.m_Thickness;

		if (a.m_ComponentFlags.m_Flags != b.m_ComponentFlags.m_Flags)
			diff["ComponentFlags"] = b.m_ComponentFlags.m_Flags;

		return diff;
	}


	void SpriteRenderSystem::ViewSpriteComponent(Entity& entity)
	{
		SpriteComponent& sprite = entity.Get<SpriteComponent>();
		NSpriteSourceManager* spritesrcManager = NSpriteSourceManager::Instance();
		NTextureManager* texureManager = NTextureManager::Instance();
		NMeshManager* meshManager = NMeshManager::Instance();
		NShaderManager* shaderManager = NShaderManager::Instance();


		uint8_t& flags = sprite.m_ComponentFlags.m_Flags;

		auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("Sprite", flags, ComponentFlags_Enabled);

		if (remove)
		{
			NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<SpriteComponent>::GetID()));
		}

		if (!open)
			return;

		if (!enabled)
			ImGui::BeginDisabled();


		ImGui::DragFloat("Thickness", &sprite.m_Thickness, 0.02f, 0.01f, 1.0f);

		ImGui::DragFloat("Fade", &sprite.m_Fade, 0.001f, 0);

		const auto& meshNames = meshManager->GetResourceNames();

		if (sprite.m_MeshNameID)
		{
			const std::string& meshName = STRFROM(sprite.m_MeshNameID);
			if (ImGui::BeginCombo("Select Mesh", meshName.c_str()))
			{
				for (const auto& name : meshNames)
				{
					bool isSelected = meshName == name;

					if (ImGui::Selectable(name.c_str(), isSelected)) {
						sprite.m_MeshNameID = STRID(name);
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
						sprite.m_MeshNameID = STRID(name);
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


		auto* spriteSource = sprite.m_SpriteSourceNameID ? spritesrcManager->Get(sprite.m_SpriteSourceNameID) : nullptr;
		auto* emissiveSpriteSource = sprite.m_EmissiveSpriteSourceNameID ? spritesrcManager->Get(sprite.m_EmissiveSpriteSourceNameID) : nullptr;


		DrawDragDrop("Main", sprite.m_SpriteSourceNameID, spriteSource, texureManager, spritesrcManager);
		DrawDragDrop("Emissive", sprite.m_EmissiveSpriteSourceNameID, emissiveSpriteSource, texureManager, spritesrcManager);

		if (spriteSource)
		{

			ImGui::Text("Source Dimensions");

			if (ImGui::DragInt(std::string(std::string("Rows")).c_str(), &spriteSource->Rows(), 0.5f, 1))
			{
				if (emissiveSpriteSource)
				{
					//auto* emissiveSpriteSource = spritesrcManager->Get(sprite.m_EmissiveSpriteSourceNameID);
					emissiveSpriteSource->Rows() = spriteSource->GetRows();
				}
			}


			if (ImGui::DragInt(std::string(std::string("Cols")).c_str(), &spriteSource->Cols(), 0.5f, 1))
			{
				if (emissiveSpriteSource)
				{
					//auto* emissiveSpriteSource = spritesrcManager->Get(sprite.m_EmissiveSpriteSourceNameID);

					emissiveSpriteSource->Cols() = spriteSource->GetCols();
				}
			}


			ImGui::DragInt(std::string(std::string("Frame")).c_str(), reinterpret_cast<int*>(&sprite.m_FrameIndex), 1, 0, spriteSource->GetFrameCount());
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Sprite Source not found");
		}


		ImGui::ColorEdit4("Tint", glm::value_ptr(sprite.m_Color));
		ImGui::ColorEdit4("Emissive Color", glm::value_ptr(sprite.m_EmissiveColor));


		ImGui::DragFloat("Emissive Strength", &sprite.m_EmissiveStrength, 0.001f, 0);


		if (!enabled)
			ImGui::EndDisabled();

		if (!entity.Has<TransformComponent>())
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Requires a Transform component");


		ImGui::TreePop();


	}

	void SpriteRenderSystem::DrawDragDrop(const char* label, uint32_t& nameID, SpriteSource*& source, NTextureManager* texMgr, NSpriteSourceManager* srcMgr)
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
				nameID = 0;
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
						nameID = STRID(name);
						ImGui::CloseCurrentPopup();
					}
				}
			}

			ImGui::EndChild();
			ImGui::EndPopup();
		}
	}

}