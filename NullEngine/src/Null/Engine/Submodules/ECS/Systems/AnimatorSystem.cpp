//------------------------------------------------------------------------------
//
// File Name:	AnimatorSystem.cpp
// Author(s):	YourName
//
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "AnimatorSystem.h"
#include "imgui.h"
#include <nlohmann/json.hpp>
#include "../../../../Tools/ImGuiH.h"
#include <NIncludes.h>

namespace NULLENGINE
{
	AnimatorSystem::AnimatorSystem()
	{
		Require < AnimatorComponent > ();

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register < AnimatorComponent > (CreateAnimatorComponent,
			[this](Entity& entity) { this->ViewAnimatorComponent(entity); },
			WriteAnimatorComponent, AddAnimatorComponent, DiffAnimatorComponent);
	}

	void AnimatorSystem::Load()
	{
	}

	void AnimatorSystem::Init()
	{
		ISystem::Init();

		// Register events here if needed
	}

	void AnimatorSystem::Update(float dt)
	{
		// Implement update logic here
	}

	void AnimatorSystem::RuntimeUpdate(float dt)
	{
		NRegistry* registry = NRegistry::Instance();

		for (auto entity : GetSystemEntities())
		{


			auto& animator = registry->GetComponent<AnimatorComponent>(entity);
			auto& animation = registry->GetComponent<AnimationComponent>(entity);


			// Skip if disabled
			if (!animator.componentFlags.IsSet(ComponentFlags_Enabled))
				continue;

			// Check if we need to switch clips
			if (animator.currentClipID != animator.lastClipID)
			{
				SwitchToClip(Entity(entity, registry), animator, animation);
				animator.lastClipID = animator.currentClipID;
			}
		}
	}

	void AnimatorSystem::Render()
	{
	}

	void AnimatorSystem::Unload()
	{
	}

	void AnimatorSystem::Shutdown()
	{
	}

	void AnimatorSystem::RegisterToScripAPI(sol::state& lua)
	{
		// Register Lua bindings here
		lua.new_usertype<AnimatorComponent>("AnimatorComponent",
			"currentClipID", &AnimatorComponent::currentClipID,
			"frameSpeedModifier", &AnimatorComponent::frameSpeedModifier
		);
	}


	void AnimatorSystem::CreateAnimatorComponent(void* component, const nlohmann::json& json)
	{
		auto* comp = static_cast<AnimatorComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			comp->currentClipID = STRID(jsonWrapper.GetString("currentClip", ""));
			comp->frameSpeedModifier = jsonWrapper.GetFloat("frameSpeedModifier", 1.0f);

			comp->componentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", comp->componentFlags.m_Flags);

			comp->flags.m_Flags = jsonWrapper.GetUInt8("AnimatorFlags", 0);
		}
	}

	void AnimatorSystem::AddAnimatorComponent(void* component, NRegistry* registry, EntityID id)
	{
		// Add or update component in registry
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<AnimatorComponent*>(component);



		componentFactory->AddOrUpdate<AnimatorComponent>(id, comp, registry, comp->currentClipID, comp->lastClipID, comp->frameSpeedModifier, comp->flags, comp->componentFlags);

	}

	JSON AnimatorSystem::WriteAnimatorComponent(const void* component)
	{
		nlohmann::json json;

		auto& comp = *static_cast<const AnimatorComponent*>(component);



		return json;
	}

	JSON AnimatorSystem::DiffAnimatorComponent(const void* base, const void* modified)
	{
		// Calculate diff between base and modified components
		return JSON{};
	}


	bool AnimatorSystem::PlayClip(Entity entity, uint32_t clipID)
	{
		auto& animator = entity.Get<AnimatorComponent>();

		// Check if we have an animation controller for validation
		if (entity.Has<AnimationControllerComponent>())
		{
			auto& controller = entity.Get<AnimationControllerComponent>();
			if (!CanTransition(controller, animator.currentClipID, clipID))
			{
				return false; // Invalid transition
			}
		}

		// Valid transition or no controller - switch clips
		animator.currentClipID = clipID;
		return true;
	}

	void AnimatorSystem::SwitchToClip(Entity entity, AnimatorComponent& animator, AnimationComponent& animation)
	{
		// Get the clip directly from ResourceManager
		auto* clip = NAnimationClipManager::Instance()->Get(animator.currentClipID);
		if (!clip)
			return;

		// Use the AnimationSystem function with preserve frame flag from clip
		bool preserveFrame = clip->flags.IsSet(AnimationFlags_PreserveFrame);

		AnimationSystem::PlayAnimation(entity,
			clip->frameCount,
			clip->frameDuration * animator.frameSpeedModifier,
			clip->flags.IsSet(AnimationFlags_IsLooping),
			clip->startingFrame,
			clip->flags.IsSet(AnimationFlags_IsReversed),
			clip->flags.IsSet(AnimationFlags_IsPingPong),
			preserveFrame
		);
	}

	bool AnimatorSystem::CanTransition(const AnimationControllerComponent& controller, uint32_t fromClip, uint32_t toClip)
	{
		//auto* stateMachine = NAnimationClipManager::Instance()->Get(controller.);
		//if (!stateMachine)
		//	return true; // No state machine = no restrictions

		//return stateMachine->CanTransition(fromClip, toClip);

		return false; // Placeholder, implement actual transition logic
	}


	void AnimatorSystem::ViewAnimatorComponent(Entity& entity)
	{
		AnimatorComponent& component = entity.Get<AnimatorComponent>();
		uint8_t& flags = component.componentFlags.m_Flags;

		auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("AnimatorComponent", flags, ComponentFlags_Enabled);

		if (remove)
		{
			NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<AnimatorComponent>::GetID()));
			return;
		}

		if (!open)
			return;

		if (!enabled)
			ImGui::BeginDisabled();

		// Current clip ID
		ImGui::Text("Current Clip ID: %s", STRFROM(component.currentClipID));

		// Frame speed modifier
		ImGui::SliderFloat("Frame Speed Modifier", &component.frameSpeedModifier, 0.1f, 3.0f);

		// In ViewAnimatorComponent:

		// Get current sprite's sprite source to filter relevant clips
		uint32_t currentSpriteSourceID = 0;
		if (entity.Has<SpriteComponent>())
		{
			SpriteComponent& sprite = entity.Get<SpriteComponent>();
			currentSpriteSourceID = sprite.spriteSourceNameID;
		}

		// Get available clips for this sprite source
		auto availableClips = NAnimationClipManager::Instance()->GetClipsForSpriteSource(currentSpriteSourceID);

		if (!availableClips.empty())
		{
			// Find current selection index
			int currentSelection = -1;
			for (size_t i = 0; i < availableClips.size(); ++i)
			{
				if (availableClips[i]->nameID == component.currentClipID)
				{
					currentSelection = static_cast<int>(i);
					break;
				}
			}

			// Create dropdown with clip names
			if (ImGui::BeginCombo("Animation Clip", currentSelection >= 0 ?
				STRFROM(availableClips[currentSelection]->nameID).c_str() : "None"))
			{
				for (size_t i = 0; i < availableClips.size(); ++i)
				{
					bool isSelected = (currentSelection == static_cast<int>(i));
					std::string clipName = STRFROM(availableClips[i]->nameID);

					if (ImGui::Selectable(clipName.c_str(), isSelected))
					{
						PlayClip(entity, availableClips[i]->nameID);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No clips available for current sprite source");
		}

		if (!enabled)
			ImGui::EndDisabled();

		ImGui::TreePop();
	}
}
