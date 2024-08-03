
//------------------------------------------------------------------------------
//
// File Name:	ScriptSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "ScriptSystem.h"
#include "imgui.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <box2d/b2_body.h>
#include <sol/sol.hpp>


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//


namespace fs = std::filesystem;
using LuaValue = std::variant<float, int, std::string, bool, sol::table, sol::function, std::monostate>;


//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{

	LuaValue GetValue(const sol::object& value) {
		if (value.is<float>()) {
			return value.as<float>();
		}
		else if (value.is<int>()) {
			return value.as<int>();
		}
		else if (value.is<std::string>()) {
			return value.as<std::string>();
		}
		else if (value.is<bool>()) {
			return value.as<bool>();
		}
		else if (value.is<sol::table>()) {
			return value.as<sol::table>();
		}
		else if (value.is<sol::function>()) {
			return value.as<sol::function>();
		}
		else {
			return std::monostate{};
		}
	}

	void SetValue(sol::table& table, const std::string& key, const LuaValue& value) {
		std::visit([&table, &key](auto&& arg) {
			table[key] = arg;
			}, value);
	}

	void ImGuiDisplayLuaValue(const std::string& key, const LuaValue& value) {
		std::visit([&key](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, float>) {
				ImGui::Text("%s: %f", key.c_str(), arg);
			}
			else if constexpr (std::is_same_v<T, int>) {
				ImGui::Text("%s: %d", key.c_str(), arg);
			}
			else if constexpr (std::is_same_v<T, std::string>) {
				ImGui::Text("%s: %s", key.c_str(), arg.c_str());
			}
			else if constexpr (std::is_same_v<T, bool>) {
				ImGui::Text("%s: %s", key.c_str(), arg ? "true" : "false");
			}
			//else if constexpr (std::is_same_v<T, sol::table>) {
			//	ImGui::Text("%s: (table)", key.c_str());
			//}
			//else if constexpr (std::is_same_v<T, sol::function>) {
			//	ImGui::Text("%s: (function)", key.c_str());
			//}
	/*		else {
				ImGui::Text("%s: (unknown type)", key.c_str());
			}*/
			}, value);
	}

	void ImGuiDisplayAndModifyLuaValue(sol::table& table, const std::string& key, LuaValue& value) {
		std::visit([&table, &key, &value](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, float>) {
				float v = arg;
				if (ImGui::InputFloat(key.c_str(), &v)) {
					value = v;
					SetValue(table, key, value);
				}
			}
			else if constexpr (std::is_same_v<T, int>) {
				int v = arg;
				if (ImGui::InputInt(key.c_str(), &v)) {
					value = v;
					SetValue(table, key, value);
				}
			}
			else if constexpr (std::is_same_v<T, std::string>) {
				char buffer[256];
				std::strncpy(buffer, arg.c_str(), sizeof(buffer));
				buffer[sizeof(buffer) - 1] = 0;
				if (ImGui::InputText(key.c_str(), buffer, sizeof(buffer))) {
					value = std::string(buffer);
					SetValue(table, key, value);
				}
			}
			else if constexpr (std::is_same_v<T, bool>) {
				bool v = arg;
				if (ImGui::Checkbox(key.c_str(), &v)) {
					value = v;
					SetValue(table, key, value);
				}
			}
			//else if constexpr (std::is_same_v<T, sol::table>) {
			//	ImGui::Text("%s: (table)", key.c_str());
			//}
			//else if constexpr (std::is_same_v<T, sol::function>) {
			//	ImGui::Text("%s: (function)", key.c_str());
			//}
			//else {
			//	ImGui::Text("%s: (unknown type)", key.c_str());
			//}
			}, value);
	}

	ScriptSystem::ScriptSystem()
	{
		Require<ScriptComponent>();

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		componentFactory->Register<ScriptComponent>(CreateScriptComponent,
			[this](Entity& id) { this->ViewScriptComponent(id); }, WriteScriptComponent);

	}


	void ScriptSystem::Load()
	{
	}

	void ScriptSystem::Init()
	{
		ISystem::Init();


		for (const auto entityId : GetSystemEntities())
		{
			InitializeScripts(entityId);
		}

		//NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();


		//SUBSCRIBE_EVENT(WindowResizeEvent, &ScriptSystem::OnWindowResize, eventManager);
	}

	void ScriptSystem::Update(float dt)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();


		for (const auto entityId : GetSystemEntities())
		{
			ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(entityId);

			for (auto& script : scriptComponent.m_Scripts) {
				if (script.valid()) 
				{
					sol::function update_func = script["Update"];
					if (update_func.valid()) 
					{
						try 
						{
							// Call the Lua Update function with the entity and delta time
							update_func(script, dt);
						}
						catch (const sol::error& e) 
						{
							NLE_CORE_ERROR("Lua Error: {0}",e.what());
						}
					}
				}
			}
		}
	}


	void ScriptSystem::Render() 
	{
	}

	void ScriptSystem::Unload()
	{
	}

	void ScriptSystem::Shutdown()
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		for (const auto entityId : GetSystemEntities())
		{

			ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(entityId);

			for (auto& script : scriptComponent.m_Scripts) {
				if (script.valid()) {
					sol::function exit_func = script["Exit"];
					if (exit_func.valid()) {
						exit_func();
					}
				}
			}
		}
	}

	void ScriptSystem::RegisterToScripAPI(sol::state& lua)
	{
	}


	void ScriptSystem::CreateScriptComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		auto* comp = static_cast<ScriptComponent*>(component);
		JsonReader jsonWrapper(json);

		std::vector<std::string> script_names;

		if (!jsonWrapper.Empty())
		{
			//std::string scriptPath = jsonWrapper.GetString("lua_script");
			if (json.contains("scripts") && json["scripts"].is_array())
			{
				for (const auto& script_name : json["scripts"])
				{
					if (script_name.is_string())
					{
						script_names.push_back(script_name.get<std::string>());
					}
				}
			}
		}

		componentFactory->AddOrUpdate<ScriptComponent>(id, comp, registry, script_names);
	}

	JSON ScriptSystem::WriteScriptComponent(BaseComponent* component)
	{
		nlohmann::json json;

		auto& script = *static_cast<ScriptComponent*>(component);

		json["scripts"] = script.m_Script_Names;

		return json;
	}

	void ScriptSystem::ViewScriptComponent(Entity& entity)
	{
		ScriptComponent& script = entity.Get<ScriptComponent>();

		for (const auto& scriptName : script.m_Script_Names)
		{

		}

		for (size_t i = 0; i < script.m_Script_Names.size(); i++)
		{
			sol::table scr = script.m_Scripts[i];

			for (auto& pair : scr) 
			{
				sol::object key = pair.first;
				sol::object value = pair.second;

				std::string keyStr = key.as<std::string>();
				LuaValue luaValue = GetValue(value);
				ImGuiDisplayAndModifyLuaValue(scr, keyStr, luaValue);
			}
		}
	

	}
	void ScriptSystem::InitializeScripts(EntityID id)
	{
		NScriptingInterface* scriptingInterface = m_Parent->Get< NScriptingInterface>();
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NSceneManager* scene = NEngine::Instance().Get<NSceneManager>();

		ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(id);

		scriptComponent.m_Lua_state.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string, sol::lib::io);

		std::string scriptDirectory = "../Assets/Scripts/";  // Update this path to your script directory

		//AddScriptPaths(scriptComponent.m_Lua_state, scriptDirectory);

		scriptComponent.m_Lua_state["ParentEntity"] = &scene->GetCurrentScene()->GetEntity(id);

		for (const auto& name: scriptComponent.m_Script_Names)
		{
			try
			{
				// Load the script using require_file
				std::string fullPath = GetScriptPaths(scriptDirectory, name);
				sol::table script = scriptComponent.m_Lua_state.require_file(name, fullPath);
				scriptComponent.m_Scripts.emplace_back(script);
			}
			catch (const std::exception& e)
			{
				NLE_CORE_ERROR("Error loading script {0}: {1}", name, e.what());
			}
		}

		scriptingInterface->RegisterEngineFunctions(scriptComponent.m_Lua_state);



		for (auto& script : scriptComponent.m_Scripts) {
			if (script.valid()) {
				sol::function start_func = script["Start"];
				if (start_func.valid()) {
					start_func();
				}
			}
		}
		
	}

	const std::string ScriptSystem::GetScriptPaths(const std::string& scriptDirectory, const std::string& scriptName)
	{
		for (const auto& entry : fs::recursive_directory_iterator(scriptDirectory))
		{
			if (entry.is_regular_file()) {
				// Compare the filename without extension
				if (entry.path().filename().stem() == scriptName) 
				{
					auto path = entry.path();
					auto pathStr = entry.path().string();
					return pathStr; // Return the full path if filename matches
				}
			}
		}


		return std::string();
	}
}