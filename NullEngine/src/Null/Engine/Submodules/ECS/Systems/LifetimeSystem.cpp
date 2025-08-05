
//------------------------------------------------------------------------------
//
// File Name:	LifetimeSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "LifetimeSystem.h"
#include "Null/Engine/Submodules/ECS/Components/LifetimeComponent.h"
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>
#include "../Entities/Entity.h"
#include "../../Scene.h"

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
	LifetimeSystem::LifetimeSystem()
	{
		Require<LifetimeComponent>();

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register<LifetimeComponent>(CreateLifetimeComponent,
			[this](Entity& id) { this->ViewLifetimeComponent(id); },
			WriteLifetimeComponent, 
			AddLifetimeComponent, DiffLifetimeComponent,
			nullptr // AssignNameToComponent is not used here, so we pass nullptr
		);

	}


	void LifetimeSystem::Load()
	{
	}

	void LifetimeSystem::Init()
	{
		ISystem::Init();

		NEventManager* eventManager =   NEventManager::Instance();


		//SUBSCRIBE_EVENT(EngineRunStateEvent, &LifetimeSystem::OnRuntimeStart, eventManager, EventPriority::Low);
	}

	void LifetimeSystem::Update(float dt)
	{

	}

	void LifetimeSystem::RuntimeUpdate(float dt)
	{
		NRegistry* registry = NRegistry::Instance();
		

		for (const auto entityId : GetSystemEntities())
		{
			LifetimeComponent& LifetimeComp = registry->GetComponent<LifetimeComponent>(entityId);
			LifetimeComp.timeRemaining -= dt;

			if (LifetimeComp.timeRemaining <= 0.0f) 
			{
				if(registry->HasComponent<DestroyedComponent>(entityId))
					continue; // Entity already marked as destroyed
				// If the entity is not already marked as destroyed, we mark it now
				registry->AddComponent<DestroyedComponent>(entityId);
			}
		}
	}


	void LifetimeSystem::Render()
	{
	}

	void LifetimeSystem::RenderImGui()
	{
		ISystem::RenderImGui();
	}

	void LifetimeSystem::Unload()
	{
	}

	void LifetimeSystem::Shutdown()
	{
	}

	void LifetimeSystem::RegisterToScripAPI(sol::state& lua)
	{

	}


	bool LifetimeSystem::OnRuntimeStart(const EngineRunStateEvent& e)
	{

		return false;
	}

	void LifetimeSystem::CreateLifetimeComponent(void* component, const nlohmann::json& json)
	{
		//NComponentFactory* componentFactory = NComponentFactory::Instance();
		NCameraManager* camManager = NCameraManager::Instance();

		auto* comp = static_cast<LifetimeComponent*>(component);
		JsonReader jsonWrapper(json);
		if (!jsonWrapper.Empty())
		{
			comp->timeRemaining = jsonWrapper.GetFloat("timeRemaining", 1.0f);
			ComponentFlagSet flags;
			flags.Set(ComponentFlags_Enabled);
			flags.Set(ComponentFlags_Serialized);
			comp->componentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);

		}

	}

	void LifetimeSystem::AddLifetimeComponent(void* component, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<LifetimeComponent*>(component);
		componentFactory->AddOrUpdate<LifetimeComponent>(id, comp, registry, comp->timeRemaining);
	}


	JSON LifetimeSystem::WriteLifetimeComponent(const void* component)
	{
		nlohmann::json json;

		auto& comp = *static_cast<const LifetimeComponent*>(component);

		if (!comp.componentFlags.IsSet(ComponentFlags_Serialized))
			return json;

		json["Lifetime"]["timeRemaining"] = comp.timeRemaining;
		json["Lifetime"]["ComponentFlags"] = comp.componentFlags.m_Flags;


		return json;
	}

	JSON LifetimeSystem::DiffLifetimeComponent(const void* base, const void* modified)
	{
		auto* a = static_cast<const LifetimeComponent*>(base);
		auto* b = static_cast<const LifetimeComponent*>(modified);

		JSON diff;
		JSON lifetimeJson;

		if (a->timeRemaining != b->timeRemaining)
			lifetimeJson["timeRemaining"] = b->timeRemaining;


		if (a->componentFlags.m_Flags != b->componentFlags.m_Flags)
			diff["ComponentFlags"] = b->componentFlags.m_Flags;

		if (!lifetimeJson.empty())
			diff["Lifetime"] = lifetimeJson;

		return diff;
	}

	void LifetimeSystem::ViewLifetimeComponent(Entity& entity)
	{
		if (!entity.Has<LifetimeComponent>())
			return;




		auto& LifetimeComp = entity.Get<LifetimeComponent>();


		uint8_t& flags = LifetimeComp.componentFlags.m_Flags;
		// Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
		auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("Transform", flags, ComponentFlags_Enabled);

		if (remove)
		{
			NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<LifetimeComponent>::GetID()));
		}

		if (!open)
			return;

		if (!enabled)
			ImGui::BeginDisabled();

		ImGui::DragFloat("Lifetime", &LifetimeComp.timeRemaining, 0.001f, 0);


		if (!enabled)
			ImGui::EndDisabled();




		ImGui::TreePop();

	}


}