
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

	static void SaveLuaTableToFile(sol::table& table, const std::string& filename, const std::string& path);
	static std::string SerializeLuaTable(sol::table& table);
	static LuaValue GetValue(const sol::object& value);
	static void SetValue(sol::table& table, const std::string& key, const LuaValue& value);
	static void ImGuiDisplayAndModifyLuaValue(sol::table& dataOwner, sol::table& table, const std::string& key, LuaValue& value);
	static std::string SerializeLuaFunctions(sol::table& table, const std::string& tableName);
	static JSON GenerateScriptDifferences(const sol::table& script, const std::unordered_map<std::string, LuaValue>& defaults);
	static JSON LuaValueToJson(const LuaValue& luaValue);
	static LuaValue JsonToLuaValue(const JSON& jValue);

	LuaValue JsonToLuaValue(const JSON& jValue) {
		if (jValue.is_number_integer()) {
			return LuaValue(jValue.get<int>());
		}
		else if (jValue.is_boolean()) {
			return LuaValue(jValue.get<bool>());
		}
		else if (jValue.is_string()) {
			return LuaValue(jValue.get<std::string>());
		}
		else if (jValue.is_number_float()) {
			return LuaValue(static_cast<float>(jValue.get<double>()));
		}
		else {
			throw std::runtime_error("Unsupported JSON value type");
		}
	}
	JSON LuaValueToJson(const LuaValue& luaValue) 
	{
		return std::visit([&luaValue](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, float>) {
				float v = arg;
				return JSON(v);
			}
			else if constexpr (std::is_same_v<T, int>) {
				int v = arg;
				return JSON(v);
			}
			else if constexpr (std::is_same_v<T, std::string>) {
				char buffer[256];
				std::string v = arg;
				return JSON(v);
			}
			else if constexpr (std::is_same_v<T, bool>) {
				bool v = arg;
				return JSON(v);
			}
			else
				return JSON();
		}, luaValue);
	}


	JSON GenerateScriptDifferences(const sol::table& script, const std::unordered_map<std::string, LuaValue>& defaults)
	{
		JSON differences;
		for (auto& pair : script) {
			sol::object key = pair.first;
			sol::table props = pair.second;

			sol::object value = props["value"];
			bool serialize = props["serialize"];

			if (value.is<sol::function>() || value.is<sol::table>() || !serialize)
				continue;

			std::string keyStr = key.as<std::string>();
			LuaValue currentValue = GetValue(value);

			//auto it = defaults.find(keyStr);
			//if (it != defaults.end()) {
			//	const LuaValue& defaultValue = it->second;
			//	if (currentValue != defaultValue) {
			//		differences[keyStr] = LuaValueToJson(currentValue);
			//	}
			//}
			//else {
			//	differences[keyStr] = LuaValueToJson(currentValue);
			//}

			differences[keyStr] = LuaValueToJson(currentValue);
		}

		return differences;
	}

	void SetValue(sol::table& table, const std::string& key, const LuaValue& value)
	{
		std::visit([&table, &key](auto&& arg)
			{
				table[key] = arg;
			}, value);
	}

	LuaValue GetValue(const sol::object& value)
	{
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
			return  value.as<sol::table>();
		}
		else if (value.is<sol::function>()) {
			return value.as<sol::function>();
		}
		else {
			return std::monostate{};
		}
	}

	std::string SerializeLuaFunctions(sol::table& table, const std::string& tableName) {
		std::stringstream ss;
		for (auto& pair : table) {
			sol::object key = pair.first;
			sol::object value = pair.second;

			if (value.is<sol::function>()) {
				ss << "function " << tableName << ":" << key.as<std::string>() << "()\n";
				ss << "    -- function body\n";
				ss << "end\n\n";
			}
		}
		return ss.str();
	}

	std::string SerializeLuaTable(sol::table& table, const std::string& tableName) {
		std::stringstream ss;
		ss << tableName << " = {\n";
		for (auto& pair : table) {
			sol::object key = pair.first;
			sol::object value = pair.second;
			if (value.is<sol::function>()) {
				continue; // Skip functions
			}
			ss << "    ";
			if (key.is<std::string>()) {
				ss << key.as<std::string>() << " = ";
			}
			else if (key.is<int>()) {
				ss << "[" << key.as<int>() << "] = ";
			}

			if (value.is<float>()) {
				ss << value.as<float>();
			}
			else if (value.is<int>()) {
				ss << value.as<int>();
			}
			else if (value.is<std::string>()) {
				ss << "\"" << value.as<std::string>() << "\"";
			}
			else if (value.is<bool>()) {
				ss << (value.as<bool>() ? "true" : "false");
			}
			else if (value.is<sol::table>()) {
				sol::table nestedTable = value.as<sol::table>();
				ss << SerializeLuaTable(nestedTable, key.as<std::string>());
			}

			ss << ",\n";
		}
		ss << "}";
		return ss.str();
	}

	void SaveLuaTableToFile(sol::table& table, const std::string& filename, const std::string& path)
	{
		// Read the original file content
		std::ifstream file(path);
		if (!file.is_open()) {
			NLE_CORE_ERROR("Unable to open file {0}", filename);
			return;
		}

		std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();

		// Serialize the updated table
		std::string serializedTable = SerializeLuaTable(table, filename);


		// Find the position of the table name
		std::size_t startPos = fileContent.find(filename);
		if (startPos == std::string::npos) {
			std::cerr << "Table name not found in the file" << std::endl;
			return;
		}

		// Find the position of the opening brace
		startPos = fileContent.find('{', startPos);
		if (startPos == std::string::npos) {
			std::cerr << "Opening brace not found for the table" << std::endl;
			return;
		}

		// Find the position of the closing brace
		std::size_t endPos = fileContent.find('}', startPos);
		if (endPos == std::string::npos) {
			std::cerr << "Closing brace not found for the table" << std::endl;
			return;
		}

		// Adjust endPos to include the closing brace
		endPos++;

		// Create the updated content by removing the old table and inserting the new one
		std::string updatedContent = serializedTable + fileContent.substr(endPos);
		//+ filename + " = " + serializedTable + "\n\n" + fileContent.substr(endPos);

	// Write the updated content back to the file
		std::ofstream outFile(path);
		if (!outFile.is_open()) {
			NLE_CORE_ERROR("Unable to open file {0} for writing", filename);
			return;
		}
		outFile << updatedContent;
		outFile.close();

		NLE_CORE_INFO("Table updated in ", filename);

	}


	void ImGuiDisplayAndModifyLuaValue(sol::table& dataOwner, sol::table& table, const std::string& key, LuaValue& value)
	{
		std::visit([&dataOwner, &key, &value](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, float>) {
				float v = arg;
				if (ImGui::InputFloat(key.c_str(), &v)) {
					value = v;
					SetValue(dataOwner, key, value);
				}
			}
			else if constexpr (std::is_same_v<T, int>) {
				int v = arg;
				if (ImGui::InputInt(key.c_str(), &v)) {
					value = v;
					SetValue(dataOwner, key, value);
				}
			}
			else if constexpr (std::is_same_v<T, std::string>) {
				char buffer[256];
				std::strncpy(buffer, arg.c_str(), sizeof(buffer));
				buffer[sizeof(buffer) - 1] = 0;
				if (ImGui::InputText(key.c_str(), buffer, sizeof(buffer))) {
					value = std::string(buffer);
					SetValue(dataOwner, key, value);
				}
			}
			else if constexpr (std::is_same_v<T, bool>) {
				bool v = arg;
				if (ImGui::Checkbox(key.c_str(), &v)) {
					value = v;
					SetValue(dataOwner, key, value);
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
							NLE_CORE_ERROR("Lua Error: {0}", e.what());
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

		//std::vector<std::string> script_names;

		if (!jsonWrapper.Empty())
		{

			if (json.contains("scripts") )
			{	
				for (const auto& [scriptName, Defaults] : json["scripts"].items())
				{
					for (const auto& [varName, value] : Defaults.items())
					{
						comp->m_ScriptDefaults[scriptName][varName] = JsonToLuaValue(value);
					}
					comp->m_Script_Names.push_back(scriptName);
				}
			}
			else 
			{
				NLE_CORE_ERROR("'scripts' key is missing or not an array.");
			}
		}

		componentFactory->AddOrUpdate<ScriptComponent>(id, comp, registry, comp->m_Script_Names, comp->m_ScriptDefaults);
	}
	


	JSON ScriptSystem::WriteScriptComponent(BaseComponent* component)
	{
		nlohmann::json json;

		auto& script = *static_cast<ScriptComponent*>(component);

		//json["Script"]["scripts"] = script.m_Script_Names;


		JSON defaultsJSON;

		for (size_t i = 0; i < script.m_Script_Names.size(); ++i) {
			const std::string& scriptName = script.m_Script_Names[i];
			const auto& defaults = script.m_ScriptDefaults.at(scriptName);
			const sol::table& scriptTable = script.m_Scripts[i]["data"];

			nlohmann::json scriptDiffs = GenerateScriptDifferences(scriptTable, defaults);
			defaultsJSON["scripts"][scriptName] = scriptDiffs;
		}


		json["Script"] = defaultsJSON;


		//for (size_t i = 0; i < script.m_Script_Names.size(); i++)
		//{
		//	sol::table scr = script.m_Scripts[i];

		//	for (auto& pair : scr)
		//	{
		//		sol::object key = pair.first;
		//		sol::object value = pair.second;

		//		std::string keyStr = key.as<std::string>();
		//		LuaValue luaValue = GetValue(value);
		//		ImGuiDisplayAndModifyLuaValue(scr, keyStr, luaValue);
		//	}
		//}

		return json;
	}

	void ScriptSystem::ViewScriptComponent(Entity& entity)
	{
		ScriptComponent& script = entity.Get<ScriptComponent>();

		//for (const auto& scriptName : script.m_Script_Names)
		//{

		//}

		for (size_t i = 0; i < script.m_Script_Names.size(); i++)
		{
			sol::table scr = script.m_Scripts[i];

			sol::table data = scr["data"];
			for (auto& pair : data)
			{
				sol::object key = pair.first;
				sol::table props = pair.second;

				sol::object value = props["value"];
				bool serialize = props["serialize"];

				std::string keyStr = key.as<std::string>();
				LuaValue luaValue = GetValue(value);

				if(serialize)
					ImGuiDisplayAndModifyLuaValue(scr,data, keyStr, luaValue);
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

		for (const auto& name : scriptComponent.m_Script_Names)
		{
			try
			{
				// Load the script using require_file
				std::string fullPath = GetScriptPaths(scriptDirectory, name);
				scriptComponent.m_Script_Paths.push_back(fullPath);
				sol::table script = scriptComponent.m_Lua_state.require_file(name, fullPath);

				sol::table data = script["data"];
				for (auto& pair : data)
				{
					sol::object key = pair.first;
					sol::table props = pair.second;

					sol::object value = props["value"];
					bool serialize = props["serialize"];

					std::string keyStr = key.as<std::string>();
					LuaValue luaValue = GetValue(value);

					if (value.is<sol::function>() || value.is<sol::table>() || !serialize)
						continue;


					if (scriptComponent.m_ScriptDefaults.empty())
					{
						scriptComponent.m_ScriptDefaults[name][keyStr] = luaValue;
						continue;
					}

					// Get the script default for the given name, creating it if it doesn't exist
					auto& defaultsForName = scriptComponent.m_ScriptDefaults[name];

					// Check if the key exists in the defaults for the name
					if (defaultsForName.contains(keyStr))
					{
						// If the value has changed, set it; otherwise, update the default
						if (defaultsForName.at(keyStr) != luaValue)
						{
							SetValue(script, keyStr, defaultsForName.at(keyStr));
						}
						else
						{
							defaultsForName[keyStr] = luaValue;
						}
					}
					else
					{
						defaultsForName[keyStr] = luaValue;
					}


				}


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