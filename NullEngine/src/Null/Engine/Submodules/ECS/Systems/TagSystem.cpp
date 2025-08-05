
//------------------------------------------------------------------------------
//
// File Name:	TagSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "TagSystem.h"
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>

#include "../Entities/Entity.h"
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
	TagSystem::TagSystem()
	{
		m_tags = {
			// Entity Type / Role Tags
			STRID("Player"),
			STRID("Enemy"),
			STRID("Interactable"),
			STRID("Collectible"),
			STRID("Trigger"),

			// Editor & Engine Behavior Tags
			STRID("EditorOnly"),
			STRID("HideInHierarchy"),
			STRID("DoNotSerialize"),
			STRID("RuntimeGenerated"),
			STRID("Persistent"),
			STRID("IgnoreForPrefab"),

			// Gameplay Tags
			STRID("Checkpoint"),
			STRID("UI"),
			STRID("PhysicsIgnored")
		};



	}


	void TagSystem::Load()
	{
	}

	void TagSystem::Init()
	{
		ISystem::Init();

		NEventManager* eventManager =   NEventManager::Instance();


		//SUBSCRIBE_EVENT(EngineRunStateEvent, &TagSystem::OnRuntimeStart, eventManager, EventPriority::Low);
	}

	void TagSystem::Update(float dt)
	{
	}

	void TagSystem::RuntimeUpdate(float dt)
	{

	}


	void TagSystem::Render()
	{
	}

	void TagSystem::RenderImGui()
	{
		ISystem::RenderImGui();
	}

	void TagSystem::Unload()
	{
	}

	void TagSystem::Shutdown()
	{
	}

	void TagSystem::RegisterToScripAPI(sol::state& lua)
	{
		lua.new_usertype<TagComponent>(
			"Tag",
			sol::no_constructor,
			"type_id", &Component<TagComponent>::GetID,

			// lambda has_tag to check if tag exists
			"has_tag", [](TagComponent& tagComp, const std::string& tag) {
				for (uint8_t i = 0; i < tagComp.tagCount; ++i)
					if (tagComp.tagIDs[i] == STRID(tag))
						return true;
				return false;
			}
		);

	}


	bool TagSystem::OnRuntimeStart(const EngineRunStateEvent& e)
	{

		return false;
	}

	void TagSystem::CreateTagComponent(void* component, const nlohmann::json& json)
	{
		auto* comp = static_cast<TagComponent*>(component);
		JsonReader jsonWrapper(json);

		comp->tagCount = 0; // clear any previous data

		if (!jsonWrapper.Empty())
		{
			auto tagsVec = jsonWrapper.GetArray<std::string>("tags", {});

			for (const auto& tag : tagsVec)
			{
				if (comp->tagCount >= TagComponent::MaxTags)
					break;

				uint32_t tagID = STRID(tag);

				// Avoid duplicates
				bool duplicate = false;
				for (uint8_t i = 0; i < comp->tagCount; ++i)
				{
					if (comp->tagIDs[i] == tagID)
					{
						duplicate = true;
						break;
					}
				}
				if (duplicate)
					continue;

				comp->tagIDs[comp->tagCount++] = tagID;
			}



			ComponentFlagSet flags;
			flags.Set(ComponentFlags_Enabled);
			flags.Set(ComponentFlags_Serialized);

			comp->componentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);
		
		}
	}


	void TagSystem::AddTagComponent(void* component, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<TagComponent*>(component);

		componentFactory->AddOrUpdate<TagComponent>(id, comp, registry, comp->tagIDs, comp->tagCount, comp->componentFlags);

	}

	JSON TagSystem::WriteTagComponent(const void* component)
	{
		nlohmann::json json;
		const auto& tagComp = *static_cast<const TagComponent*>(component);

		if (!tagComp.componentFlags.IsSet(ComponentFlags_Serialized))
			return json;

		json["Tag"]["tags"] = nlohmann::json::array();

		for (uint8_t i = 0; i < tagComp.tagCount; ++i)
		{
			const uint32_t tagID = tagComp.tagIDs[i];
			const std::string tagName = STRFROM(tagID); // resolve hash to name
			json["Tag"]["tags"].push_back(tagName);
		}
		json["Tag"]["ComponentFlags"] = tagComp.componentFlags.m_Flags;

		return json;
	}

	JSON TagSystem::DiffTagComponent(const void* base, const void* modified)
	{
		JSON diff;

		auto& a = *static_cast<const TagComponent*>(base);
		auto& b = *static_cast<const TagComponent*>(modified);


		if (a.componentFlags.m_Flags != b.componentFlags.m_Flags)
			diff["ComponentFlags"] = b.componentFlags.m_Flags;

		if (a.tagCount != b.tagCount)
		{
			diff["tags"] = nlohmann::json::array();
			for (uint8_t i = 0; i < b.tagCount; ++i)
			{
				diff["tags"].push_back(STRFROM(b.tagIDs[i]));
			}
		}
		else
		{
			bool anyDifference = false;
			for (uint8_t i = 0; i < b.tagCount; ++i)
			{
				bool found = false;
				for (uint8_t j = 0; j < a.tagCount; ++j)
				{
					if (a.tagIDs[j] == b.tagIDs[i])
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					anyDifference = true;
					break;
				}
			}

			if (anyDifference)
			{
				diff["tags"] = nlohmann::json::array();
				for (uint8_t i = 0; i < b.tagCount; ++i)
				{
					diff["tags"].push_back(STRFROM(b.tagIDs[i]));
				}
			}
		}

		return diff;
	}


	void TagSystem::ViewTagComponent(Entity& entity)
	{
		if (!entity.Has<TagComponent>())
			return;

		auto& tagComp = entity.Get<TagComponent>();

		ImGui::Text("Tags:");
		ImGui::Spacing();

		std::vector<uint32_t> tagsToRemove;

		for (uint8_t i = 0; i < tagComp.tagCount; ++i)
		{
			const uint32_t tagID = tagComp.tagIDs[i];
			const std::string tagName = STRFROM(tagID);

			ImGui::PushID(tagID);
			ImGui::Text("%s", tagName.c_str()); ImGui::SameLine();
			if (ImGui::Button("Remove"))
			{
				tagsToRemove.push_back(tagID);
			}
			ImGui::PopID();
		}


		for (uint32_t removeID : tagsToRemove)
		{
			for (uint8_t i = 0; i < tagComp.tagCount; ++i)
			{
				if (tagComp.tagIDs[i] == removeID)
				{
					// Shift down
					for (uint8_t j = i; j < tagComp.tagCount - 1; ++j)
						tagComp.tagIDs[j] = tagComp.tagIDs[j + 1];

					--tagComp.tagCount;
					break;
				}
			}
		}


		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		static char newTagBuffer[64] = { 0 };
		ImGui::PushItemWidth(150);
		ImGui::InputText("New Tag", newTagBuffer, IM_ARRAYSIZE(newTagBuffer));
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("Add##text"))
		{
			if (strlen(newTagBuffer) > 0 && tagComp.tagCount < TagComponent::MaxTags)
			{
				uint32_t tagID = STRID(newTagBuffer);

				// Check for duplicates
				bool exists = false;
				for (uint8_t i = 0; i < tagComp.tagCount; ++i)
					if (tagComp.tagIDs[i] == tagID)
						exists = true;

				if (!exists)
				{
					tagComp.tagIDs[tagComp.tagCount++] = tagID;
					m_tags.insert(STRID(std::string(newTagBuffer))); // update tag pool
				}

				newTagBuffer[0] = '\0';
			}
		}


		ImGui::Spacing();
		ImGui::Text("Add from existing tags:");

		static int selected = -1;
		std::vector<const char*> tagNames;
		tagNames.reserve(m_tags.size());

		for (const auto& tag : m_tags)
			tagNames.push_back(STRFROM(tag).c_str());

		if (!tagNames.empty())
		{
			ImGui::PushItemWidth(150);
			ImGui::Combo("Available Tags", &selected, tagNames.data(), static_cast<int>(tagNames.size()));
			ImGui::PopItemWidth();
			ImGui::SameLine();
			if (ImGui::Button("Add##dropdown") && selected >= 0 && tagComp.tagCount < TagComponent::MaxTags)
			{
				uint32_t tagID = STRID(tagNames[selected]);

				// Check for duplicates
				bool exists = false;
				for (uint8_t i = 0; i < tagComp.tagCount; ++i)
					if (tagComp.tagIDs[i] == tagID)
						exists = true;

				if (!exists)
					tagComp.tagIDs[tagComp.tagCount++] = tagID;
			}
		}

	}
}