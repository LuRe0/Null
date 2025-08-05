//------------------------------------------------------------------------------
//
// File Name:	AnimationControllerSystem.cpp
// Author(s):	YourName
//
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "AnimationControllerSystem.h"
#include "imgui.h"
#include <nlohmann/json.hpp>
#include "../../../../Tools/ImGuiH.h"
#include "NIncludes.h"

namespace NULLENGINE
{
	AnimationControllerSystem::AnimationControllerSystem()
	{
		Require < AnimationControllerComponent > ();

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register < AnimationControllerComponent > (CreateAnimationControllerComponent,
			[this](Entity& entity) { this->ViewAnimationControllerComponent(entity); },
			WriteAnimationControllerComponent, AddAnimationControllerComponent, DiffAnimationControllerComponent);
	}

	void AnimationControllerSystem::Load()
	{
	}

	void AnimationControllerSystem::Init()
	{
		ISystem::Init();

		// Register events here if needed
	}

	void AnimationControllerSystem::Update(float dt)
	{
		// Implement update logic here
	}

	void AnimationControllerSystem::RuntimeUpdate(float dt)
	{
		Update(dt);
	}

	void AnimationControllerSystem::Render()
	{
	}

	void AnimationControllerSystem::Unload()
	{
	}

	void AnimationControllerSystem::Shutdown()
	{
	}

	void AnimationControllerSystem::RegisterToScripAPI(sol::state& lua)
	{
		// Register Lua bindings here
		lua.new_usertype<AnimationControllerComponent>("AnimationController",
			"stateMachineID", &AnimationControllerComponent::stateMachineID,
			"currentStateID", &AnimationControllerComponent::currentStateID
		);
	}

	void AnimationControllerSystem::CreateAnimationControllerComponent(void* component, const nlohmann::json& json)
	{
		auto* comp = static_cast<AnimationControllerComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			comp->stateMachineID = STRID(jsonWrapper.GetString("stateMachine", ""));
			comp->currentStateID = STRID(jsonWrapper.GetString("currentState", ""));

			ComponentFlagSet flags;
			flags.Set(ComponentFlags_Enabled);
			flags.Set(ComponentFlags_Serialized);
			comp->componentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);
		}
	}

	void AnimationControllerSystem::AddAnimationControllerComponent(void* component, NRegistry* registry, EntityID id)
	{
		// Add or update component in registry
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<AnimationControllerComponent*>(component);



		componentFactory->AddOrUpdate<AnimationControllerComponent>(id, comp, registry, comp->stateMachineID, comp->currentStateID, comp->componentFlags);

	}
	JSON AnimationControllerSystem::WriteAnimationControllerComponent(const void* component)
	{
		nlohmann::json json;
		auto& comp = *static_cast<const AnimationControllerComponent*>(component);

		if (!comp.componentFlags.IsSet(ComponentFlags_Serialized))
			return json;

		json["AnimationController"]["stateMachine"] = STRFROM(comp.stateMachineID);
		json["AnimationController"]["currentState"] = STRFROM(comp.currentStateID);
		json["AnimationController"]["ComponentFlags"] = comp.componentFlags.m_Flags;

		return json;
	}

	JSON AnimationControllerSystem::DiffAnimationControllerComponent(const void* base, const void* modified)
	{
		// Calculate diff between base and modified components
		return JSON{};
	}

	void AnimationControllerSystem::ViewAnimationControllerComponent(Entity& entity)
	{
		AnimationControllerComponent& component = entity.Get<AnimationControllerComponent>();
		uint8_t& flags = component.componentFlags.m_Flags;

		auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("AnimationControllerComponent", flags, ComponentFlags_Enabled);

		if (remove)
		{
			NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<AnimationControllerComponent>::GetID()));
			return;
		}

		if (!open)
			return;

		if (!enabled)
			ImGui::BeginDisabled();

		// Current state display
		ImGui::Text("Current State: %s", STRFROM(component.currentStateID));

		// State Machine selection
		auto availableStateMachineNames = NAnimationGraphManager::Instance()->GetResourceNames();

		if (!availableStateMachineNames.empty())
		{
			// Find current selection
			int currentSelection = -1;
			std::string currentStateMachineName = STRFROM(component.stateMachineID);

			for (size_t i = 0; i < availableStateMachineNames.size(); ++i)
			{
				if (availableStateMachineNames[i] == currentStateMachineName)
				{
					currentSelection = static_cast<int>(i);
					break;
				}
			}

			// State machine dropdown
			std::string displayName = (currentSelection >= 0) ? currentStateMachineName : "None";

			if (ImGui::BeginCombo("State Machine", displayName.c_str()))
			{
				// None option
				if (ImGui::Selectable("None", currentSelection == -1))
				{
					component.stateMachineID = 0;
				}

				for (size_t i = 0; i < availableStateMachineNames.size(); ++i)
				{
					bool isSelected = (currentSelection == static_cast<int>(i));
					const std::string& stateMachineName = availableStateMachineNames[i];

					if (ImGui::Selectable(stateMachineName.c_str(), isSelected))
					{
						component.stateMachineID = STRID(stateMachineName);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
		else
		{
			ImGui::Text("No state machines available");
		}

		// Show transition rules for current state machine
		if (component.stateMachineID != 0)
		{
			auto* stateMachine = NAnimationGraphManager::Instance()->Get(component.stateMachineID);
			if (stateMachine && ImGui::TreeNode("Transition Rules"))
			{
				// Show allowed transitions from current state
				if (component.currentStateID != 0)
				{
					const auto& allowedTransitions = stateMachine->GetAllowedTransitions(component.currentStateID);

					ImGui::Text("From '%s' can transition to:", STRFROM(component.currentStateID));
					ImGui::Indent();

					if (allowedTransitions.empty())
					{
						ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "No transitions defined");
					}
					else
					{
						for (uint32_t toClipID : allowedTransitions)
						{
							ImGui::BulletText("%s", STRFROM(toClipID));
						}
					}
					ImGui::Unindent();
				}

				// Show bypass clips
				const auto& bypassClips = stateMachine->GetBypassClips();
				if (!bypassClips.empty())
				{
					ImGui::Separator();
					ImGui::Text("Bypass Clips (can interrupt anything):");
					ImGui::Indent();
					for (uint32_t clipID : bypassClips)
					{
						ImGui::BulletText("%s", STRFROM(clipID));
					}
					ImGui::Unindent();
				}

				ImGui::TreePop();
			}
		}

		if (!enabled)
			ImGui::EndDisabled();

		ImGui::TreePop();
	}
}
