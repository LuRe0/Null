
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
#include "NIncludes.h"

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
			AddSpriteComponent, DiffSpriteComponent,
			nullptr // AssignNameToComponent is not used here, so we pass nullptr
		);
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

			if (!sprite.componentFlags.IsSet(ComponentFlags_Enabled))
				continue;

			//if (!camManager->IsWithinFrustum(transform.m_Translation, (transform.m_Scale/2.0f)))
			//	continue;

			glm::vec4 worldPosition = transform.transformMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

			// Transform the world position to camera space
			glm::vec4 cameraSpacePosition = viewMatrix * worldPosition;

			// The depth is the z-component of the camera space position
			float depth = cameraSpacePosition.z;


			Mesh* mesh = sprite.meshNameID ? NMeshManager::Instance()->Get(sprite.meshNameID) : nullptr;
			SpriteSource* src = sprite.spriteSourceNameID ? NSpriteSourceManager::Instance()->Get(sprite.spriteSourceNameID)  : nullptr;
			SpriteSource* emissiveSrc = sprite.emissiveSpriteSourceNameID ? NSpriteSourceManager::Instance()->Get(sprite.emissiveSpriteSourceNameID) : nullptr;

			if (sprite.color.a < 1.0f)
			{
				renderer->AddRenderCall(RenderCommandTypes::Transparent, std::make_unique<ElementData>(transform.transformMatrix, mesh, src, sprite.color, "",
					sprite.frameIndex, entityId, sprite.thickness, sprite.fade, RenderData::INSTANCED, -depth));
			}
			else
			{
				renderer->AddRenderCall(RenderCommandTypes::Opaque, std::make_unique<ElementData>(transform.transformMatrix, mesh, src, sprite.color, "",
					sprite.frameIndex, entityId, sprite.thickness, sprite.fade, RenderData::INSTANCED, -depth));
			}


			SpriteSource* selectedSrc = emissiveSrc != nullptr ? emissiveSrc : src;
			if (sprite.emissiveStrength > 0.0f)
			{
				glm::mat4 emissiveTransform = transform.transformMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.01f));
				glm::vec4 emissiveTint = sprite.emissiveColor * sprite.emissiveStrength;
				renderer->AddRenderCall(RenderCommandTypes::Emissive, std::make_unique<ElementData>(
					emissiveTransform,
					mesh,
					selectedSrc,
					emissiveTint,
					"",
					sprite.frameIndex,
					entityId,
					sprite.thickness,
					sprite.fade,
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
				"frame_index", sol::readonly(&SpriteComponent::frameIndex),
				"tint", sol::readonly(&SpriteComponent::color),
				"emissive_color", sol::readonly(&SpriteComponent::emissiveColor),
				"emissive_strength", sol::readonly(&SpriteComponent::emissiveStrength),
				"get_alpha", sol::overload(
					[](SpriteComponent& sprite)
					{
						return sprite.color.a;
					}
				),
				"set_alpha", sol::overload(
					[](SpriteComponent& sprite, float a)
					{
						sprite.color = glm::vec4(sprite.color.r, sprite.color.g, sprite.color.b, a);
					}
				),
				"set_tint", sol::overload(
					[](SpriteComponent& sprite, float r, float g, float b, float a)
					{
						sprite.color = glm::vec4(r, g, b, a);
					},
					[](SpriteComponent& sprite, const glm::vec4& newColor)
					{
						sprite.color = newColor;
					}
				),
				"set_emissive_color", sol::overload(
					[](SpriteComponent& sprite, float r, float g, float b, float a)
					{
						sprite.emissiveColor = glm::vec4(r, g, b, a);
					},
					[](SpriteComponent& sprite, const glm::vec4& newColor)
					{
						sprite.emissiveColor = newColor;
					}
				),
				"set_emissive_strength", sol::overload(
					[](SpriteComponent& sprite, float s)
					{
						sprite.emissiveStrength = s;
					}
				),
				"set_frame_index", [](SpriteComponent& sprite, int i)
				{
					sprite.frameIndex = i;
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
			comp->fade = jsonWrapper.GetFloat("fade", 0.005f);
			comp->thickness = std::clamp(comp->thickness, 0.02f, jsonWrapper.GetFloat("thickness", 1.0f));

			comp->frameIndex = jsonWrapper.GetInt("frameindex", 0);
			comp->emissiveStrength = jsonWrapper.GetFloat("emissiveStrength", 0);
			glm::vec2 dimension = jsonWrapper.GetVec2("dimension", { 1.0f, 1.0f });
			auto src = jsonWrapper.GetString("texture", "");
			if (!src.empty())
			{
				comp->spriteSourceNameID = STRID(src);
				spritesrcManager->Create(src, static_cast<int>(dimension.x), static_cast<int>(dimension.y));
			}

			src = jsonWrapper.GetString("emissiveTexture", "");
			if (!src.empty())
			{
				comp->emissiveSpriteSourceNameID = STRID(src);
				spritesrcManager->Create(src, static_cast<int>(dimension.x), static_cast<int>(dimension.y));
			}



			const std::string& meshName = jsonWrapper.GetString("meshname", "");

			comp->meshNameID = !meshName.empty() ? STRID(meshName) : STRID("Quad");


			comp->color = jsonWrapper.GetVec4("tint", { 1.0f, 1.0f, 1.0f, 1.0f });
			comp->emissiveColor = jsonWrapper.GetVec4("emissiveColor", { 1.0f, 1.0f, 1.0f, 1.0f });


			ComponentFlagSet flags;
			flags.Set(ComponentFlags_Enabled);
			flags.Set(ComponentFlags_Serialized);
			comp->componentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);
		}

	}

	void SpriteRenderSystem::AddSpriteComponent(void* component, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<SpriteComponent*>(component);


		componentFactory->AddOrUpdate<SpriteComponent>(id, comp, registry, comp->color, comp->emissiveColor, comp->frameIndex, comp->spriteSourceNameID,
			comp->emissiveSpriteSourceNameID, comp->meshNameID, comp->emissiveStrength, comp->thickness, comp->fade, comp->componentFlags);

	}

	JSON SpriteRenderSystem::WriteSpriteComponent(const void* component)
	{
		nlohmann::json json;

		auto& sprite = *static_cast<const SpriteComponent*>(component);

		if (!sprite.componentFlags.IsSet(ComponentFlags_Serialized))
			return json;

		json["Sprite"]["frameindex"] = sprite.frameIndex;
		json["Sprite"]["texture"] = STRFROM(sprite.spriteSourceNameID);
		json["Sprite"]["emissiveTexture"] = STRFROM(sprite.emissiveSpriteSourceNameID);
		auto* src = sprite.spriteSourceNameID ? NSpriteSourceManager::Instance()->Get(sprite.spriteSourceNameID) : nullptr;
		if (src)
		{
			json["Sprite"]["dimension"] = nlohmann::json::array({ src->GetRows(), src->GetCols() });
		}
		else
		{
			json["Sprite"]["dimension"] = nlohmann::json::array({ 1, 1 });
		}

		json["Sprite"]["tint"] = { sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a };
		json["Sprite"]["emissiveColor"] = { sprite.emissiveColor.r, sprite.emissiveColor.g, sprite.emissiveColor.b, sprite.emissiveColor.a };
		json["Sprite"]["meshname"] = STRFROM(sprite.meshNameID);
		json["Sprite"]["fade"] = sprite.fade;
		json["Sprite"]["emissiveStrength"] = sprite.emissiveStrength;
		json["Sprite"]["thickness"] = sprite.thickness;
		json["Sprite"]["ComponentFlags"] = sprite.componentFlags.m_Flags;

		return json;

	}

	JSON SpriteRenderSystem::DiffSpriteComponent(const void* base, const void* modified)
	{
		JSON diff;

		auto& b = *static_cast<const SpriteComponent*>(modified);
		auto& a = *static_cast<const SpriteComponent*>(base);

		if (a.frameIndex != b.frameIndex)
			diff["frameindex"] = b.frameIndex;

		const std::string texA = a.spriteSourceNameID ? STRFROM(a.spriteSourceNameID) : "";
		const std::string texB = b.spriteSourceNameID ? STRFROM(b.spriteSourceNameID) : "";
		if (texA != texB)
			diff["texture"] = texB;

		const std::string emisTexA = a.emissiveSpriteSourceNameID ? STRFROM(a.emissiveSpriteSourceNameID) : "";
		const std::string emisTexB = b.emissiveSpriteSourceNameID ? STRFROM(b.emissiveSpriteSourceNameID) : "";
		if (emisTexA != emisTexB)
			diff["emissiveTexture"] = emisTexB;

		if (a.spriteSourceNameID && b.spriteSourceNameID)
		{
			auto* srcA = NSpriteSourceManager::Instance()->Get(a.spriteSourceNameID);
			auto* srcB = NSpriteSourceManager::Instance()->Get(b.spriteSourceNameID);

			if (srcA->GetRows() != srcB->GetRows() ||
				srcA->GetCols() != srcB->GetCols())
			{
				diff["dimension"] = { srcB->GetRows(), srcB->GetCols() };
			}
		}

		if (a.color != b.color)
			diff["tint"] = { b.color.r, b.color.g, b.color.b, b.color.a };

		if (a.emissiveColor != b.emissiveColor)
			diff["emissiveColor"] = { b.emissiveColor.r, b.emissiveColor.g, b.emissiveColor.b, b.emissiveColor.a };


		const std::string meshA = a.meshNameID ? STRFROM(a.meshNameID) : "";
		const std::string meshB = b.meshNameID ? STRFROM(a.meshNameID) : "";
		if (meshA != meshB)
			diff["meshname"] = meshB;

		if (a.fade != b.fade)
			diff["fade"] = b.fade;

		if (a.emissiveStrength != b.emissiveStrength)
			diff["emissiveStrength"] = b.emissiveStrength;

		if (a.thickness != b.thickness)
			diff["thickness"] = b.thickness;

		if (a.componentFlags.m_Flags != b.componentFlags.m_Flags)
			diff["ComponentFlags"] = b.componentFlags.m_Flags;

		return diff;
	}


	void SpriteRenderSystem::ViewSpriteComponent(Entity& entity)
	{
		SpriteComponent& sprite = entity.Get<SpriteComponent>();
		NSpriteSourceManager* spritesrcManager = NSpriteSourceManager::Instance();
		NTextureManager* texureManager = NTextureManager::Instance();
		NMeshManager* meshManager = NMeshManager::Instance();
		NShaderManager* shaderManager = NShaderManager::Instance();


		uint8_t& flags = sprite.componentFlags.m_Flags;

		auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("Sprite", flags, ComponentFlags_Enabled);

		if (remove)
		{
			NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<SpriteComponent>::GetID()));
		}

		if (!open)
			return;

		if (!enabled)
			ImGui::BeginDisabled();


		ImGui::DragFloat("Thickness", &sprite.thickness, 0.02f, 0.01f, 1.0f);

		ImGui::DragFloat("Fade", &sprite.fade, 0.001f, 0);

		const auto& meshNames = meshManager->GetResourceNames();

		if (sprite.meshNameID)
		{
			const std::string& meshName = STRFROM(sprite.meshNameID);
			if (ImGui::BeginCombo("Select Mesh", meshName.c_str()))
			{
				for (const auto& name : meshNames)
				{
					bool isSelected = meshName == name;

					if (ImGui::Selectable(name.c_str(), isSelected)) {
						sprite.meshNameID = STRID(name);
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
						sprite.meshNameID = STRID(name);
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


		auto* spriteSource = sprite.spriteSourceNameID ? spritesrcManager->Get(sprite.spriteSourceNameID) : nullptr;
		auto* emissiveSpriteSource = sprite.emissiveSpriteSourceNameID ? spritesrcManager->Get(sprite.emissiveSpriteSourceNameID) : nullptr;


		ImGuiH::DrawDragDrop("Main", sprite.spriteSourceNameID, spriteSource, texureManager, spritesrcManager);
		ImGuiH::DrawDragDrop("Emissive", sprite.emissiveSpriteSourceNameID, emissiveSpriteSource, texureManager, spritesrcManager);

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


			ImGui::DragInt(std::string(std::string("Frame")).c_str(), reinterpret_cast<int*>(&sprite.frameIndex), 1, 0, spriteSource->GetFrameCount());
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Sprite Source not found");
		}


		ImGui::ColorEdit4("Tint", glm::value_ptr(sprite.color));
		ImGui::ColorEdit4("Emissive Color", glm::value_ptr(sprite.emissiveColor));


		ImGui::DragFloat("Emissive Strength", &sprite.emissiveStrength, 0.001f, 0);


		if (!enabled)
			ImGui::EndDisabled();

		if (!entity.Has<TransformComponent>())
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Requires a Transform component");


		ImGui::TreePop();


	}

}