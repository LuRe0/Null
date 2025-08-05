//------------------------------------------------------------------------------
//
// File Name:	NameSystem.cpp
// Author(s):	YourName
//
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "NameSystem.h"
#include "imgui.h"
#include <nlohmann/json.hpp>
#include "../../../../Tools/ImGuiH.h"
#include <NIncludes.h>

namespace NULLENGINE
{
	NameSystem::NameSystem()
	{
		Require < NameComponent >();

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register < NameComponent >(CreateNameComponent,
			[this](Entity& entity) { this->ViewNameComponent(entity); },
			WriteNameComponent, AddNameComponent, DiffNameComponent);
	}

	void NameSystem::Load()
	{
	}

	void NameSystem::Init()
	{
		ISystem::Init();

		// Register events here if needed
	}

	void NameSystem::Update(float dt)
	{
		// Implement update logic here
	}

	void NameSystem::RuntimeUpdate(float dt)
	{
		Update(dt);
	}

	void NameSystem::Render()
	{
	}

	void NameSystem::Unload()
	{
	}

	void NameSystem::Shutdown()
	{
	}

	void NameSystem::RegisterToScripAPI(sol::state& lua)
	{
		// Register Lua bindings here
	}

	void NameSystem::CreateNameComponent(void* component, const nlohmann::json& json)
	{
		// Deserialize from json to component
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<NameComponent*>(component);


		JsonReader jsonWrapper(json);

		if (!jsonWrapper.Empty())
		{
			comp->nameID = STRID(jsonWrapper.GetString("name", ""));
			ComponentFlagSet flags;
			flags.Set(ComponentFlags_Enabled);
			flags.Set(ComponentFlags_Serialized);
			comp->componentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);
		}

	}

	void NameSystem::AddNameComponent(void* component, NRegistry* registry, EntityID id)
	{
		// Add or update component in registry
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<NameComponent*>(component);

		componentFactory->AddOrUpdate<NameComponent>(id, comp, registry, comp->nameID, comp->componentFlags);

	}

	JSON NameSystem::WriteNameComponent(const void* component)
	{
		nlohmann::json json;

		auto& name = *static_cast<const NameComponent*>(component);


		if (!name.componentFlags.IsSet(ComponentFlags_Serialized))
			return json;

		json["Name"]["name"] = STRFROM(name.nameID);
		json["Name"]["componentFlags"] = name.componentFlags.m_Flags;

		return json;
	}

	JSON NameSystem::DiffNameComponent(const void* base, const void* modified)
	{
		// Calculate diff between base and modified components
		return JSON{};
	}

	void NameSystem::ViewNameComponent(Entity& entity)
	{
		//// Access the name component
		NameComponent& name = entity.Get<NameComponent>();
		//uint8_t& flags = name.componentFlags.m_Flags;
		//// Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
		// Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
		auto open = ImGuiH::CollapsingHeader("Name");

		if (!open)
			return;

		/// // ImGui controls for the component properties
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.5f, 1.0f), "Name: %s", STRFROM(name.nameID).c_str());

		ImGui::TreePop();
	}
}
