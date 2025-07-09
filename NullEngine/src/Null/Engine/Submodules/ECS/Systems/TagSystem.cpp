
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
			[this](Entity& id) { this->ViewTagComponent(id); }, WriteTagComponent);

		m_tags =
		{
			// Entity Type / Role Tags
			"Player",
			"Enemy",
			"Interactable",
			"Collectible",
			"Trigger",

			// Editor & Engine Behavior Tags
			"EditorOnly",
			"HideInHierarchy",
			"DoNotSerialize",
			"RuntimeGenerated",
			"Persistent",
			"IgnoreForPrefab",

			// Gameplay Tags
			"Checkpoint",
			"UI",
			"PhysicsIgnored"
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
			for (const auto& tag : tagComp.m_Tags)
			{
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
				return tagComp.m_Tags.find(tag) != tagComp.m_Tags.end();
			},

			// lambda to add a tag
			"add_tag", [](TagComponent& tagComp, const std::string& tag) {
				tagComp.m_Tags.insert(tag);
			},

			// lambda to remove a tag
			"remove_tag", [](TagComponent& tagComp, const std::string& tag) {
				tagComp.m_Tags.erase(tag);
			}
		);

	}


	bool TagSystem::OnRuntimeStart(const EngineRunStateEvent& e)
	{

		return false;
	}

	void TagSystem::CreateTagComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();
		NCameraManager* camManager = NCameraManager::Instance();

		auto* comp = static_cast<TagComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			auto tagsVec = jsonWrapper.GetArray<std::string>("tags", std::vector<std::string>());
			std::set<std::string> tagsSet(tagsVec.begin(), tagsVec.end());
			comp->m_Tags = std::move(tagsSet);
		}

		componentFactory->AddOrUpdate<TagComponent>(id, comp, registry, comp->m_Tags);
	}

	JSON TagSystem::WriteTagComponent(BaseComponent* component)
	{
		nlohmann::json json;

		auto& cam = *static_cast<TagComponent*>(component);
		json["Tag"]["tags"] = nlohmann::json::array();

		for (const auto& tag : cam.m_Tags) 
		{
			json["Tag"]["tags"].push_back(tag);
		}

		return json;
	}
	void TagSystem::ViewTagComponent(Entity& entity)
	{
		if (!entity.Has<TagComponent>())
			return;

		auto& tagComp = entity.Get<TagComponent>();

		ImGui::Text("Tags:");
		ImGui::Spacing();

		// Display current tags with [X] remove buttons
		std::vector<std::string> tagsToRemove;
		for (const auto& tag : tagComp.m_Tags)
		{
			ImGui::PushID(tag.c_str());
			ImGui::Text("%s", tag.c_str()); ImGui::SameLine();
			if (ImGui::Button("Remove"))
			{
				tagsToRemove.push_back(tag);
			}
			ImGui::PopID();
		}

		for (const auto& tag : tagsToRemove)
		{
			tagComp.m_Tags.erase(tag);
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
			if (strlen(newTagBuffer) > 0)
			{
				tagComp.m_Tags.insert(std::string(newTagBuffer));
				m_tags.insert(std::string(newTagBuffer));
				newTagBuffer[0] = '\0'; // clear input
			}
		}

		ImGui::Spacing();
		ImGui::Text("Add from existing tags:");

		static int selected = -1;
		std::vector<const char*> tagNames;
		tagNames.reserve(m_tags.size());
		for (const auto& tag : m_tags)
			tagNames.push_back(tag.c_str());

		if (!tagNames.empty())
		{
			ImGui::PushItemWidth(150);
			ImGui::Combo("Available Tags", &selected, tagNames.data(), static_cast<int>(tagNames.size()));
			ImGui::PopItemWidth();
			ImGui::SameLine();
			if (ImGui::Button("Add##dropdown") && selected >= 0)
			{
				tagComp.m_Tags.insert(tagNames[selected]);
			}
		}
	}


}