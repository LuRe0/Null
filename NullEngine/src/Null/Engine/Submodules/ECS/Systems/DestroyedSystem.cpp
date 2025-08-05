//------------------------------------------------------------------------------
//
// File Name:	DestroyedSystem.cpp
// Author(s):	YourName
//
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "DestroyedSystem.h"
#include "imgui.h"
#include <nlohmann/json.hpp>
#include "../../../../Tools/ImGuiH.h"
#include "NIncludes.h"

namespace NULLENGINE
{
	DestroyedSystem::DestroyedSystem()
	{
		Require < DestroyedComponent > ();

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register < DestroyedComponent > (CreateDestroyedComponent,
			[this](Entity& entity) { this->ViewDestroyedComponent(entity); },
			WriteDestroyedComponent, AddDestroyedComponent, DiffDestroyedComponent);
	}

	void DestroyedSystem::Load()
	{
	}

	void DestroyedSystem::Init()
	{
		ISystem::Init();

		// Register events here if needed
	}

	void DestroyedSystem::Update(float dt)
	{
		// Implement update logic here
	}

	void DestroyedSystem::RuntimeUpdate(float dt)
	{
		Update(dt);
	}

	void DestroyedSystem::Render()
	{
	}

	void DestroyedSystem::Unload()
	{
	}

	void DestroyedSystem::Shutdown()
	{
	}

	void DestroyedSystem::RegisterToScripAPI(sol::state& lua)
	{
		// Register Lua bindings here
	}

	void DestroyedSystem::CreateDestroyedComponent(void* component, const nlohmann::json& json)
	{
		// Deserialize from json to component

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<DestroyedComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			
			// Deserialize component properties


			ComponentFlagSet flags;
			flags.Set(ComponentFlags_Enabled);
			flags.Set(ComponentFlags_Serialized);	

			comp->componentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);
		}


	}

	void DestroyedSystem::AddDestroyedComponent(void* component, NRegistry* registry, EntityID id)
	{
		// Add or update component in registry
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<DestroyedComponent*>(component);



		componentFactory->AddOrUpdate<DestroyedComponent>(id, comp, registry);

	}

	JSON DestroyedSystem::WriteDestroyedComponent(const void* component)
	{
		nlohmann::json json;

		auto& comp = *static_cast<const DestroyedComponent*>(component);



		return json;
	}

	JSON DestroyedSystem::DiffDestroyedComponent(const void* base, const void* modified)
	{
		// Calculate diff between base and modified components
		return JSON{};
	}

	void DestroyedSystem::ViewDestroyedComponent(Entity& entity)
	{
		// ImGui inspector for component
		DestroyedComponent& component = entity.Get<DestroyedComponent>();
		uint8_t& flags = component.componentFlags.m_Flags;
		// Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
		auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("DestroyedComponent", flags, ComponentFlags_Enabled);

		if (remove)
		{
			NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<DestroyedComponent>::GetID()));
		}

		if (!open)
			return;

		if (!enabled)
			ImGui::BeginDisabled();

		/// // ImGui controls for the component properties

		if (!enabled)
			ImGui::EndDisabled();




		ImGui::TreePop();
	}
}
