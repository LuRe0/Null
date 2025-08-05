//------------------------------------------------------------------------------
//
// File Name:	ArchetypeSystem.cpp
// Author(s):	YourName
//
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "ArchetypeSystem.h"
#include "imgui.h"
#include <nlohmann/json.hpp>
#include "../../../../Tools/ImGuiH.h"
#include <NIncludes.h>

namespace NULLENGINE
{
	ArchetypeSystem::ArchetypeSystem()
	{
		Require < ArchetypeComponent > ();

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register < ArchetypeComponent > (CreateArchetypeComponent,
			[this](Entity& entity) { this->ViewArchetypeComponent(entity); },
			WriteArchetypeComponent, AddArchetypeComponent, DiffArchetypeComponent);
	}

	void ArchetypeSystem::Load()
	{
	}

	void ArchetypeSystem::Init()
	{
		ISystem::Init();

		// Register events here if needed
	}

	void ArchetypeSystem::Update(float dt)
	{
		// Implement update logic here
	}

	void ArchetypeSystem::RuntimeUpdate(float dt)
	{
		Update(dt);
	}

	void ArchetypeSystem::Render()
	{
	}

	void ArchetypeSystem::Unload()
	{
	}

	void ArchetypeSystem::Shutdown()
	{
	}

	void ArchetypeSystem::RegisterToScripAPI(sol::state& lua)
	{
		// Register Lua bindings here
	}

	void ArchetypeSystem::CreateArchetypeComponent(void* component, const nlohmann::json& json)
	{
		// Deserialize from json to component

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<ArchetypeComponent*>(component);
		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			
			// Deserialize component properties

			comp->archetypeID = STRID(jsonWrapper.GetString("archetype", ""));
			comp->parentArchetypeID = STRID(jsonWrapper.GetString("parentArchetype", ""));
			// Set default flags if not present
			ComponentFlagSet flags;
			flags.Set(ComponentFlags_Enabled);
			flags.Set(ComponentFlags_Serialized);	

			comp->componentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);
		}


	}

	void ArchetypeSystem::AddArchetypeComponent(void* component, NRegistry* registry, EntityID id)
	{
		// Add or update component in registry
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<ArchetypeComponent*>(component);



		componentFactory->AddOrUpdate<ArchetypeComponent>(id, comp, registry, comp->archetypeID, comp->parentArchetypeID, comp->componentFlags);

	}

	JSON ArchetypeSystem::WriteArchetypeComponent(const void* component)
	{
		nlohmann::json json;

		auto& comp = *static_cast<const ArchetypeComponent*>(component);

		if (!comp.componentFlags.IsSet(ComponentFlags_Serialized))
			return json;

		json["Archetype"]["parentArchetype"] = STRFROM(comp.parentArchetypeID);
		json["Archetype"]["archetype"] = STRFROM(comp.archetypeID);
		json["Archetype"]["ComponentFlags"] = comp.componentFlags.m_Flags;

		return json;
	}

	JSON ArchetypeSystem::DiffArchetypeComponent(const void* base, const void* modified)
	{
		// Calculate diff between base and modified components
		return JSON{};
	}

	void ArchetypeSystem::ViewArchetypeComponent(Entity& entity)
	{
		// ImGui inspector for component
		ArchetypeComponent& component = entity.Get<ArchetypeComponent>();

		// Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
		auto open = ImGuiH::CollapsingHeader("Archetype");

		if (!open)
			return;


		/// // ImGui controls for the component properties


		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.5f, 1.0f), "Archetype: %s", STRFROM(component.archetypeID).c_str());
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.5f, 1.0f), "Parent Archetype: %s", STRFROM(component.parentArchetypeID).c_str());



		ImGui::TreePop();
	}
}
