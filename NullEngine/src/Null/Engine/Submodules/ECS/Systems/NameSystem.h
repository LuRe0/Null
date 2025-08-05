#pragma once

//------------------------------------------------------------------------------
//
// File Name:	NameSystem.h
// Author(s):	YourName
// 
//------------------------------------------------------------------------------

#include "Null/Core.h"
#include "Null/Engine/Submodules/ECS/Systems/System.h"

namespace NULLENGINE
{
	struct NameComponent;

	class NLE_API NameSystem: public ISystem < NameSystem >
	{
	public:

		NameSystem();

		void Load() override;
		void Init() override;
		void Update(float dt) override;
		virtual void RuntimeUpdate(float dt) override;

		void Render() override;

		void Unload() override;
		void Shutdown() override;

		void RegisterToScripAPI(sol::state& lua) override;

	private:

		static void CreateNameComponent(void* component, const nlohmann::json& json);
		static void AddNameComponent(void* component, NRegistry* registry, EntityID id);
		static JSON WriteNameComponent(const void* component);
		static JSON DiffNameComponent(const void* base, const void* modified);

		void ViewNameComponent(Entity& entity);
	};
}
