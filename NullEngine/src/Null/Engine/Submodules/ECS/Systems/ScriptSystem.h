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
#include "../../../../Tools/LuaTypes.h"


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
	using ScriptOverrides = std::unordered_map<uint32_t, LuaValue>;          // field -> value

	class NLE_API ScriptSystem : public ISystem<ScriptSystem>
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

		void AddScript(EntityID id, uint32_t nameID);
		void RemoveScript(EntityID id, const uint32_t& script);

		bool OnEntityCreated(const EntityCreatedEvent& e);
		bool OnEntityRemoveNamedComponent(const EntityRemoveNamedComponentEvent& e);

		bool OnScriptAdded(const ScriptCreatedEvent& e);
		bool OnScriptModified(const ScriptModifiedEvent& e);
		bool OnScriptRemoved(const ScriptRemovedEvent& e);
		bool OnSceneSwitched(const SceneSwitchEvent& e);

		bool OnCollisionEnter(const CollisionEnterEvent& e);
		bool OnCollisionExit(const CollisionExitEvent& e);


		static void CreateScriptComponent(void* component, const nlohmann::json& json);
		static void AddScriptComponent(void* component, NRegistry* registry, EntityID id);
		static void AssignNameToComponent(void* component, const uint32_t& nameID);

		static JSON WriteScriptComponent(const void* component);

		static JSON DiffScriptComponent(const void* baseComp, const void* modifiedComp);

		void ViewScriptComponent(Entity& entity);

		void InitializeScripts(EntityID id);

		const std::string GetScriptPaths(const std::string& scriptDirectory, const std::string& scriptName);


		bool m_ShowCreationMenu = false;

		std::string m_ScriptName = "New Script";

		sol::state m_LuaState;
		ImGuiTextFilter m_ScriptFilter;



		struct EntityScriptEnv {
			sol::environment env;
		};


		//using ScriptDefaults = std::unordered_map<std::string, LuaValue>;

		struct ScriptInstance
		{
			sol::table scriptTable;
		};

		
		uint32_t AddScriptOverride(const ScriptOverrides& overrides);
		void RemoveScriptOverride(uint32_t instanceID);

		uint32_t AddScriptInstance(ScriptInstance instance);
		void RemoveScriptInstance(uint32_t instanceID);
		//void RegisterScriptDefaults(uint32_t nameID, const ScriptDefaults& def);
		void AddEnvironment(EntityID entityID, EntityScriptEnv env);
		void RemoveEnvironment(EntityID entityID);

		ScriptInstance& GetInstance(uint32_t instanceID);
		ScriptOverrides& GetOverrides(uint32_t overrideID);
		EntityScriptEnv* GetEnvironment(EntityID entityID);


		std::vector<ScriptInstance> m_ScriptInstances; /*scrics will have an isntace id, each scrip comp will own a table*/
		std::vector<ScriptOverrides> m_ScriptOverrides; /* script instance ID -> overrides */
		std::unordered_map<EntityID, EntityScriptEnv> m_ScriptEnvironments; /* env will be shared between all scripts under a certain entity*/
		//std::unordered_map<uint32_t /*scriptInsanceID*/, ScriptOverrides /*lua table equivalent in cpp*/> m_ScriptOverrides;
		std::vector<uint32_t> m_FreeOverrideIDs; // For recycling indices
		std::vector<uint32_t> m_FreeTableIDs; // For recycling indices

	};

}
