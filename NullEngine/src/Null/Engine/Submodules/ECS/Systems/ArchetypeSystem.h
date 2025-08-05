#pragma once

//------------------------------------------------------------------------------
//
// File Name:	ArchetypeSystem.h
// Author(s):	YourName
// 
//------------------------------------------------------------------------------

#include "Null/Core.h"
#include "Null/Engine/Submodules/ECS/Systems/System.h"

namespace NULLENGINE
{
	struct ArchetypeComponent;

	class NLE_API ArchetypeSystem: public ISystem < ArchetypeSystem >
	{
	public:

		ArchetypeSystem();

		void Load() override;
		void Init() override;
		void Update(float dt) override;
		virtual void RuntimeUpdate(float dt) override;

		void Render() override;

		void Unload() override;
		void Shutdown() override;

		void RegisterToScripAPI(sol::state& lua) override;

	private:

		static void CreateArchetypeComponent(void* component, const nlohmann::json& json);
		static void AddArchetypeComponent(void* component, NRegistry* registry, EntityID id);
		static JSON WriteArchetypeComponent(const void* component);
		static JSON DiffArchetypeComponent(const void* base, const void* modified);

		void ViewArchetypeComponent(Entity& entity);
	};
}
