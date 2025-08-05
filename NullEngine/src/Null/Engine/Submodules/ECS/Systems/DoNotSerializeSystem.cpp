//------------------------------------------------------------------------------
//
// File Name:	DoNotSerializeSystem.cpp
// Author(s):	YourName
//
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "DoNotSerializeSystem.h"
#include "imgui.h"
#include <nlohmann/json.hpp>
#include "../../../../Tools/ImGuiH.h"
#include "NIncludes.h"


namespace NULLENGINE
{
	DoNotSerializeSystem::DoNotSerializeSystem()
	{
		Require < DoNotSerializeComponent > ();

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register < DoNotSerializeComponent > (CreateDoNotSerializeComponent,
			[this](Entity& entity) { this->ViewDoNotSerializeComponent(entity); },
			WriteDoNotSerializeComponent, AddDoNotSerializeComponent, DiffDoNotSerializeComponent);
	}

	void DoNotSerializeSystem::Load()
	{
	}

	void DoNotSerializeSystem::Init()
	{
		ISystem::Init();

		// Register events here if needed
	}

	void DoNotSerializeSystem::Update(float dt)
	{
		// Implement update logic here
	}

	void DoNotSerializeSystem::RuntimeUpdate(float dt)
	{
		Update(dt);
	}

	void DoNotSerializeSystem::Render()
	{
	}

	void DoNotSerializeSystem::Unload()
	{
	}

	void DoNotSerializeSystem::Shutdown()
	{
	}

	void DoNotSerializeSystem::RegisterToScripAPI(sol::state& lua)
	{
		// Register Lua bindings here
	}

	void DoNotSerializeSystem::CreateDoNotSerializeComponent(void* component, const nlohmann::json& json)
	{
		// Deserialize from json to component

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<DoNotSerializeComponent*>(component);
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

	void DoNotSerializeSystem::AddDoNotSerializeComponent(void* component, NRegistry* registry, EntityID id)
	{
		// Add or update component in registry
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<DoNotSerializeComponent*>(component);



		componentFactory->AddOrUpdate<DoNotSerializeComponent>(id, comp, registry);

	}

	JSON DoNotSerializeSystem::WriteDoNotSerializeComponent(const void* component)
	{
		nlohmann::json json;

		auto& comp = *static_cast<const DoNotSerializeComponent*>(component);



		return json;
	}

	JSON DoNotSerializeSystem::DiffDoNotSerializeComponent(const void* base, const void* modified)
	{
		// Calculate diff between base and modified components
		return JSON{};
	}

	void DoNotSerializeSystem::ViewDoNotSerializeComponent(Entity& entity)
	{
		// ImGui inspector for component
		DoNotSerializeComponent& component = entity.Get<DoNotSerializeComponent>();
		uint8_t& flags = component.componentFlags.m_Flags;
		// Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
		auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("DoNotSerializeComponent", flags, ComponentFlags_Enabled);

		if (remove)
		{
			NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<DoNotSerializeComponent>::GetID()));
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
