#pragma once

//------------------------------------------------------------------------------
//
// File Name:	DoNotSerializeSystem.h
// Author(s):	YourName
// 
//------------------------------------------------------------------------------

#include "Null/Core.h"
#include "Null/Engine/Submodules/ECS/Systems/System.h"

namespace NULLENGINE
{
	struct DoNotSerializeComponent;

	class NLE_API DoNotSerializeSystem: public ISystem < DoNotSerializeSystem >
	{
	public:

		DoNotSerializeSystem();

		void Load() override;
		void Init() override;
		void Update(float dt) override;
		virtual void RuntimeUpdate(float dt) override;

		void Render() override;

		void Unload() override;
		void Shutdown() override;

		void RegisterToScripAPI(sol::state& lua) override;

	private:

		static void CreateDoNotSerializeComponent(void* component, const nlohmann::json& json);
		static void AddDoNotSerializeComponent(void* component, NRegistry* registry, EntityID id);
		static JSON WriteDoNotSerializeComponent(const void* component);
		static JSON DiffDoNotSerializeComponent(const void* base, const void* modified);

		void ViewDoNotSerializeComponent(Entity& entity);
	};
}
