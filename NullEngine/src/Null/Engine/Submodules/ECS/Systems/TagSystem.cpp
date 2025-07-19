
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
		Require<TagComponent>();

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register<TagComponent>(CreateTagComponent,
			[this](Entity& id) { this->ViewTagComponent(id); },
			WriteTagComponent,
			AddTagComponent, DiffTagComponent);

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
		NRegistry* registry = NRegistry::Instance();

		for (const auto entityId : GetSystemEntities())
		{
			TagComponent& tagComp = registry->GetComponent<TagComponent>(entityId);

			for (size_t i = 0; i < tagComp.m_TagCount; i++)
			{
				auto tag = tagComp.m_TagIDs[i];
				if (!m_tags.contains(tag))
					m_tags.insert(tag);
			}
		}
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
				for (uint8_t i = 0; i < tagComp.m_TagCount; ++i)
					if (tagComp.m_TagIDs[i] == STRID(tag))
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

		comp->m_TagCount = 0; // clear any previous data

		if (!jsonWrapper.Empty())
		{
			auto tagsVec = jsonWrapper.GetArray<std::string>("tags", {});

			for (const auto& tag : tagsVec)
			{
				if (comp->m_TagCount >= TagComponent::MaxTags)
					break;

				uint32_t tagID = STRID(tag);

				// Avoid duplicates
				bool duplicate = false;
				for (uint8_t i = 0; i < comp->m_TagCount; ++i)
				{
					if (comp->m_TagIDs[i] == tagID)
					{
						duplicate = true;
						break;
					}
				}
				if (duplicate)
					continue;

				comp->m_TagIDs[comp->m_TagCount++] = tagID;
			}



			ComponentFlagSet flags;
			flags.Set(ComponentFlags_Enabled);
			flags.Set(ComponentFlags_Serialized);

			comp->m_ComponentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);
		
		}
	}


	void TagSystem::AddTagComponent(void* component, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<TagComponent*>(component);

		componentFactory->AddOrUpdate<TagComponent>(id, comp, registry, comp->m_TagIDs, comp->m_TagCount, comp->m_ComponentFlags);

	}

	JSON TagSystem::WriteTagComponent(const void* component)
	{
		nlohmann::json json;
		const auto& tagComp = *static_cast<const TagComponent*>(component);

		json["Tag"]["tags"] = nlohmann::json::array();

		for (uint8_t i = 0; i < tagComp.m_TagCount; ++i)
		{
			const uint32_t tagID = tagComp.m_TagIDs[i];
			const std::string tagName = STRFROM(tagID); // resolve hash to name
			json["Tag"]["tags"].push_back(tagName);
		}
		json["Tag"]["ComponentFlags"] = tagComp.m_ComponentFlags.m_Flags;

		return json;
	}

	JSON TagSystem::DiffTagComponent(const void* base, const void* modified)
	{
		JSON diff;

		auto& a = *static_cast<const TagComponent*>(base);
		auto& b = *static_cast<const TagComponent*>(modified);


		if (a.m_ComponentFlags.m_Flags != b.m_ComponentFlags.m_Flags)
			diff["ComponentFlags"] = b.m_ComponentFlags.m_Flags;

		if (a.m_TagCount != b.m_TagCount)
		{
			diff["tags"] = nlohmann::json::array();
			for (uint8_t i = 0; i < b.m_TagCount; ++i)
			{
				diff["tags"].push_back(STRFROM(b.m_TagIDs[i]));
			}
		}
		else
		{
			bool anyDifference = false;
			for (uint8_t i = 0; i < b.m_TagCount; ++i)
			{
				bool found = false;
				for (uint8_t j = 0; j < a.m_TagCount; ++j)
				{
					if (a.m_TagIDs[j] == b.m_TagIDs[i])
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
				for (uint8_t i = 0; i < b.m_TagCount; ++i)
				{
					diff["tags"].push_back(STRFROM(b.m_TagIDs[i]));
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

		for (uint8_t i = 0; i < tagComp.m_TagCount; ++i)
		{
			const uint32_t tagID = tagComp.m_TagIDs[i];
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
			for (uint8_t i = 0; i < tagComp.m_TagCount; ++i)
			{
				if (tagComp.m_TagIDs[i] == removeID)
				{
					// Shift down
					for (uint8_t j = i; j < tagComp.m_TagCount - 1; ++j)
						tagComp.m_TagIDs[j] = tagComp.m_TagIDs[j + 1];

					--tagComp.m_TagCount;
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
			if (strlen(newTagBuffer) > 0 && tagComp.m_TagCount < TagComponent::MaxTags)
			{
				uint32_t tagID = STRID(newTagBuffer);

				// Check for duplicates
				bool exists = false;
				for (uint8_t i = 0; i < tagComp.m_TagCount; ++i)
					if (tagComp.m_TagIDs[i] == tagID)
						exists = true;

				if (!exists)
				{
					tagComp.m_TagIDs[tagComp.m_TagCount++] = tagID;
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
			if (ImGui::Button("Add##dropdown") && selected >= 0 && tagComp.m_TagCount < TagComponent::MaxTags)
			{
				uint32_t tagID = STRID(tagNames[selected]);

				// Check for duplicates
				bool exists = false;
				for (uint8_t i = 0; i < tagComp.m_TagCount; ++i)
					if (tagComp.m_TagIDs[i] == tagID)
						exists = true;

				if (!exists)
					tagComp.m_TagIDs[tagComp.m_TagCount++] = tagID;
			}
		}

	}
}