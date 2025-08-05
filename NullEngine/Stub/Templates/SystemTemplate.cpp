//------------------------------------------------------------------------------
//
// File Name:	{{SYSTEM_NAME}}.cpp
// Author(s):	YourName
//
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "{{SYSTEM_NAME}}.h"
#include "imgui.h"
#include <nlohmann/json.hpp>
#include "../../../../Tools/ImGuiH.h"

namespace NULLENGINE
{
	{{SYSTEM_NAME}}::{{SYSTEM_NAME}}()
	{
		Require < {{COMPONENT_NAME}} > ();

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register < {{COMPONENT_NAME}} > (Create{{COMPONENT_NAME}},
			[this](Entity& entity) { this->View{{COMPONENT_NAME}}(entity); },
			Write{{COMPONENT_NAME}}, Add{{COMPONENT_NAME}}, Diff{{COMPONENT_NAME}});
	}

	void {{SYSTEM_NAME}}::Load()
	{
	}

	void {{SYSTEM_NAME}}::Init()
	{
		ISystem::Init();

		// Register events here if needed
	}

	void {{SYSTEM_NAME}}::Update(float dt)
	{
		// Implement update logic here
	}

	void {{SYSTEM_NAME}}::RuntimeUpdate(float dt)
	{
		Update(dt);
	}

	void {{SYSTEM_NAME}}::Render()
	{
	}

	void {{SYSTEM_NAME}}::Unload()
	{
	}

	void {{SYSTEM_NAME}}::Shutdown()
	{
	}

	void {{SYSTEM_NAME}}::RegisterToScripAPI(sol::state& lua)
	{
		// Register Lua bindings here
	}

	void {{SYSTEM_NAME}}::Create{{COMPONENT_NAME}}(void* component, const nlohmann::json& json)
	{
		// Deserialize from json to component

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<{{COMPONENT_NAME}}*>(component);
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

	void {{SYSTEM_NAME}}::Add{{COMPONENT_NAME}}(void* component, NRegistry* registry, EntityID id)
	{
		// Add or update component in registry
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<{{COMPONENT_NAME}}*>(component);



		componentFactory->AddOrUpdate<{{COMPONENT_NAME}}>(id, comp, registry, /*other vars*/);

	}

	JSON {{SYSTEM_NAME}}::Write{{COMPONENT_NAME}}(const void* component)
	{
		nlohmann::json json;

		auto& comp = *static_cast<const {{COMPONENT_NAME}}*>(component);



		return json;
	}

	JSON {{SYSTEM_NAME}}::Diff{{COMPONENT_NAME}}(const void* base, const void* modified)
	{
		// Calculate diff between base and modified components
		return JSON{};
	}

	void {{SYSTEM_NAME}}::View{{COMPONENT_NAME}}(Entity& entity)
	{
		// ImGui inspector for component
		{{COMPONENT_NAME}}& component = entity.Get<{{COMPONENT_NAME}}>();
		uint8_t& flags = component.componentFlags.m_Flags;
		// Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
		auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove("{{COMPONENT_NAME}}", flags, ComponentFlags_Enabled);

		if (remove)
		{
			NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveComponentEvent>(entity.GetID(), Component<{{COMPONENT_NAME}}>::GetID()));
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
