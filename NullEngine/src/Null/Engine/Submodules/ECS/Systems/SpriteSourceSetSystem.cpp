//------------------------------------------------------------------------------
//
// File Name:	SpriteSourceSetSystem.cpp
// Author(s):	YourName
//
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "SpriteSourceSetSystem.h"
#include "imgui.h"
#include <nlohmann/json.hpp>
#include "../../../../Tools/ImGuiH.h"
#include "NIncludes.h"
namespace NULLENGINE
{
	SpriteSourceSetSystem::SpriteSourceSetSystem()
	{
		Require < SpriteSourceSetComponent > ();

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register < SpriteSourceSetComponent > (CreateSpriteSourceSetComponent,
			[this](Entity& entity) { this->ViewSpriteSourceSetComponent(entity); },
			WriteSpriteSourceSetComponent, AddSpriteSourceSetComponent, DiffSpriteSourceSetComponent);
	}

	void SpriteSourceSetSystem::Load()
	{
	}

	void SpriteSourceSetSystem::Init()
	{
		ISystem::Init();

		// Register events here if needed
	}

	void SpriteSourceSetSystem::Update(float dt)
	{
		// Implement update logic here
	}

	void SpriteSourceSetSystem::RuntimeUpdate(float dt)
	{
		Update(dt);
	}

	void SpriteSourceSetSystem::Render()
	{
	}

	void SpriteSourceSetSystem::Unload()
	{
	}

	void SpriteSourceSetSystem::Shutdown()
	{
	}

	void SpriteSourceSetSystem::RegisterToScripAPI(sol::state& lua)
	{
		// Register Lua bindings here
	}

	void SpriteSourceSetSystem::CreateSpriteSourceSetComponent(void* component, const nlohmann::json& json)
	{
		// Deserialize from json to component
		NSpriteSourceManager* spritesrcManager = NSpriteSourceManager::Instance();
		SpriteSourceSetSystem* system = SpriteSourceSetSystem::Instance();
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<SpriteSourceSetComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			
			// Deserialize component properties

			if (json.contains("StartingSource") && json["StartingSource"].is_string())
			{
				comp->currentSourceID = STRID(json["StartingSource"].get<std::string>());
			}

			if (json.contains("spriteSources") && json["spriteSources"].is_array())
			{
				std::vector<SpriteSourceSet> sources;

				for (const auto& entry : json["spriteSources"])
				{

					std::string textureName;
					std::string emissiveName;
					glm::ivec2 dimension = { 1, 1 };
					// Dimensions array ignored here because manager owns dimensions

					if (entry.contains("texture") && entry["texture"].is_string())
						textureName = entry["texture"].get<std::string>();

					if (entry.contains("emissiveTexture") && entry["emissiveTexture"].is_string())
						emissiveName = entry["emissiveTexture"].get<std::string>();

					if (entry.contains("dimensions") && entry["dimensions"].is_array() && entry["dimensions"].size() == 2)
					{
						dimension.x = entry["dimensions"][0].get<int>();
						dimension.y = entry["dimensions"][1].get<int>();
					}

					uint32_t textureID = UINT32_MAX;
					uint32_t emissiveID = UINT32_MAX;

					if (!textureName.empty())
					{
						spritesrcManager->Create(textureName, static_cast<int>(dimension.x), static_cast<int>(dimension.y));
						textureID = STRID(textureName);
					}

					if (!emissiveName.empty())
						emissiveID = STRID(emissiveName);

					SpriteSourceSet srcSet;
					srcSet.spriteSourceID = textureID;
					srcSet.emissiveSpriteSourceID = emissiveID;

					sources.push_back(srcSet);
				}

				system->m_SpriteSourceSets.push_back(sources);
				comp->runtimeID = static_cast<uint32_t>(system->m_SpriteSourceSets.size() - 1);
			}
			else
			{
				comp->runtimeID = -1; // or some invalid index
			}


			ComponentFlagSet flags;
			flags.Set(ComponentFlags_Enabled);
			flags.Set(ComponentFlags_Serialized);	

			comp->componentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);
		}


	}

	void SpriteSourceSetSystem::AddSpriteSourceSetComponent(void* component, NRegistry* registry, EntityID id)
	{
		// Add or update component in registry
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<SpriteSourceSetComponent*>(component);

		componentFactory->AddOrUpdate<SpriteSourceSetComponent>(id, comp, registry, comp->currentSourceID, comp->runtimeID, comp->componentFlags);

	}

	JSON SpriteSourceSetSystem::WriteSpriteSourceSetComponent(const void* component)
	{
		nlohmann::json json;

		SpriteSourceSetSystem* system = SpriteSourceSetSystem::Instance();
		NSpriteSourceManager* spritesrcManager = NSpriteSourceManager::Instance();

		auto& comp = *static_cast<const SpriteSourceSetComponent*>(component);

		if (!comp.componentFlags.IsSet(ComponentFlags_Serialized))
			return json;

		// Top-level key matches the component name
		nlohmann::json output;
		output["StartingSource"] = STRFROM(comp.currentSourceID);
		output["ComponentFlags"] = comp.componentFlags.m_Flags;

		// Serialize the set, if valid
		if (comp.runtimeID != UINT32_MAX && comp.runtimeID < system->m_SpriteSourceSets.size())
		{
			const auto& sourceSetList = system->m_SpriteSourceSets[comp.runtimeID];
			nlohmann::json sourceArray = nlohmann::json::array();

			for (const auto& srcSet : sourceSetList)
			{
				nlohmann::json entry;

				entry["texture"] = STRFROM(srcSet.spriteSourceID);

				if (srcSet.emissiveSpriteSourceID != UINT32_MAX)
					entry["emissiveTexture"] = STRFROM(srcSet.emissiveSpriteSourceID);


				auto* src = spritesrcManager->Get(srcSet.spriteSourceID);

				entry["dimensions"] = nlohmann::json::array({ src->GetRows(), src->GetCols() });

				// Dimension not serialized here since it's owned by the sprite source
				// Optional: could fetch from manager if you want to include for completeness
				sourceArray.push_back(entry);
			}

			output["spriteSources"] = sourceArray;
		}

		json["SpriteSourceSet"] = output;
		return json;
	}


	JSON SpriteSourceSetSystem::DiffSpriteSourceSetComponent(const void* base, const void* modified)
	{
		// Calculate diff between base and modified components
		return JSON{};
	}

	void SpriteSourceSetSystem::ViewSpriteSourceSetComponent(Entity& entity)
	{
		NSpriteSourceManager* spritesrcManager = NSpriteSourceManager::Instance();
		NTextureManager* texureManager = NTextureManager::Instance();

		// ImGui inspector for component
		SpriteSourceSetComponent& component = entity.Get<SpriteSourceSetComponent>();
		uint8_t& flags = component.componentFlags.m_Flags;
		// Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
		auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("SpriteSourceSet", flags, ComponentFlags_Enabled);

		if (remove)
		{
			NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<SpriteSourceSetComponent>::GetID()));
		}

		if (!open)
			return;

		if (!enabled)
			ImGui::BeginDisabled();

		/// // ImGui controls for the component properties

		if (IsValidRuntimeIndex(component.runtimeID) || component.runtimeID < m_SpriteSourceSets.size())
		{


			auto& sourceList = m_SpriteSourceSets[component.runtimeID];

			for (size_t i = 0; i < sourceList.size(); ++i)
			{
				SpriteSourceSet& src = sourceList[i];

				ImGui::Separator();
				ImGui::PushID(i);

				auto [open, remove] = ImGuiH::CollapsingHeaderWithRemove("Sprite Source");

				if (IsValidRuntimeIndex(src.spriteSourceID))
				{
					bool isStarting = (component.currentSourceID == src.spriteSourceID);
					if (ImGui::Checkbox("Set as Starting Source", &isStarting) && isStarting)
					{
						// Only one can be starting — set to this one
						component.currentSourceID = src.spriteSourceID;
					}
				}
	
				if (remove)
				{
					sourceList.erase(sourceList.begin() + i);

					if (open)
						ImGui::TreePop();

					ImGui::PopID();
					break; // Exit loop since vector size changed
				}

				if (!open)
					continue;

				auto* spriteSource = src.spriteSourceID ? spritesrcManager->Get(src.spriteSourceID) : nullptr;
				auto* emissiveSpriteSource = src.emissiveSpriteSourceID ? spritesrcManager->Get(src.emissiveSpriteSourceID) : nullptr;


				ImGuiH::DrawDragDrop("Main", src.spriteSourceID, spriteSource, texureManager, spritesrcManager);
				ImGuiH::DrawDragDrop("Emissive", src.emissiveSpriteSourceID, emissiveSpriteSource, texureManager, spritesrcManager);

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
				}
				else
				{
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Sprite Source not found");
				}

				ImGui::TreePop();
				ImGui::PopID();
			}
		}

		ImGui::Spacing();
		if (ImGui::Button("Add Sprite Source"))
		{
			SpriteSourceSet newSrc;
			newSrc.spriteSourceID = UINT32_MAX;
			newSrc.emissiveSpriteSourceID = UINT32_MAX;
			SpriteSourceSetSystem* system = SpriteSourceSetSystem::Instance();

			if (component.runtimeID != UINT32_MAX && component.runtimeID < system->m_SpriteSourceSets.size())
			{
				// Valid index → append to existing set
				system->m_SpriteSourceSets[component.runtimeID].push_back(newSrc);
			}
			else
			{
				// Invalid or uninitialized runtimeID → create a new entry
				std::vector<SpriteSourceSet> newList = { newSrc };
				system->m_SpriteSourceSets.push_back(newList);
				component.runtimeID = static_cast<uint32_t>(system->m_SpriteSourceSets.size() - 1);
			}
		}

		if (!enabled)
			ImGui::EndDisabled();

		ImGui::TreePop();
	}
}
