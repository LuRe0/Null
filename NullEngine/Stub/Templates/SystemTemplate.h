#pragma once

//------------------------------------------------------------------------------
//
// File Name:	{{SYSTEM_NAME}}.h
// Author(s):	YourName
// 
//------------------------------------------------------------------------------

#include "Null/Core.h"
#include "Null/Engine/Submodules/ECS/Systems/System.h"

namespace NULLENGINE
{
	struct {{COMPONENT_NAME}};

	class NLE_API {{SYSTEM_NAME}}: public ISystem < {{SYSTEM_NAME}} >
	{
	public:

		{{SYSTEM_NAME}}();

		void Load() override;
		void Init() override;
		void Update(float dt) override;
		virtual void RuntimeUpdate(float dt) override;

		void Render() override;

		void Unload() override;
		void Shutdown() override;

		void RegisterToScripAPI(sol::state& lua) override;

	private:

		static void Create{{COMPONENT_NAME}}(void* component, const nlohmann::json& json);
		static void Add{{COMPONENT_NAME}}(void* component, NRegistry* registry, EntityID id);
		static JSON Write{{COMPONENT_NAME}}(const void* component);
		static JSON Diff{{COMPONENT_NAME}}(const void* base, const void* modified);

		void View{{COMPONENT_NAME}}(Entity& entity);
	};
}
