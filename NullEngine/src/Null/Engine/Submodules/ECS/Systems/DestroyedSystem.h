#pragma once

//------------------------------------------------------------------------------
//
// File Name:	DestroyedSystem.h
// Author(s):	YourName
// 
//------------------------------------------------------------------------------

#include "Null/Core.h"
#include "Null/Engine/Submodules/ECS/Systems/System.h"

namespace NULLENGINE
{
	struct DestroyedComponent;

	class NLE_API DestroyedSystem: public ISystem < DestroyedSystem >
	{
	public:

		DestroyedSystem();

		void Load() override;
		void Init() override;
		void Update(float dt) override;
		virtual void RuntimeUpdate(float dt) override;

		void Render() override;

		void Unload() override;
		void Shutdown() override;

		void RegisterToScripAPI(sol::state& lua) override;

	private:

		static void CreateDestroyedComponent(void* component, const nlohmann::json& json);
		static void AddDestroyedComponent(void* component, NRegistry* registry, EntityID id);
		static JSON WriteDestroyedComponent(const void* component);
		static JSON DiffDestroyedComponent(const void* base, const void* modified);

		void ViewDestroyedComponent(Entity& entity);
	};
}
