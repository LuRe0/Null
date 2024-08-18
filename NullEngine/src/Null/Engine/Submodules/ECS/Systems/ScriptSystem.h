#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	ScriptSystem.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Submodules/ECS/Systems/System.h"


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//


namespace NULLENGINE
{
	class NLE_API ScriptSystem : public ISystem
	{
	public:

		ScriptSystem();

		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;
		virtual void RuntimeUpdate(float dt) override;

		void Render() override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;


		void RegisterToScripAPI(sol::state& lua) override;

	private:

		void AddScript(EntityID id, const std::string& script);
		void RemoveScript(EntityID id, const std::string& script);

		bool OnScriptAdded(const ScriptCreatedEvent& e);
		bool OnScriptModified(const ScriptModifiedEvent& e);
		bool OnScriptRemoved(const ScriptRemovedEvent& e);
		bool OnSceneSwitched(const SceneSwitchEvent& e);

		static void CreateScriptComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id);

		static JSON WriteScriptComponent(BaseComponent* component);

		void ViewScriptComponent(Entity& entity);

		void InitializeScripts(EntityID id);

		const std::string GetScriptPaths(const std::string& scriptDirectory, const std::string& scriptName);


		bool m_ShowCreationMenu = false;

		std::string m_ScriptName = "New Script";

		sol::state m_LuaState;


	};

}
