
//------------------------------------------------------------------------------
//
// File Name:	NStub.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "ScriptHelper.h"
#include "imgui.h"



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void ScriptHelper::MarkScriptAsLoaded(sol::state& lua, const std::string& scriptName)
	{
		sol::table loadedScripts = lua["loadedScripts"];
		loadedScripts[scriptName] = true;
	}


	bool ScriptHelper::LoadScript(sol::state& lua, const std::string& scriptName)
	{
		if (IsScriptLoaded(lua, scriptName)) {
			NLE_CORE_WARN("Script {0}  is already loaded", scriptName);
			return false; // Script is already loaded
		}

		// Load the script and mark it as loaded
		MarkScriptAsLoaded(lua, scriptName);
		return true; // Script was just loaded
	}


	bool ScriptHelper::IsScriptLoaded(sol::state& lua, const std::string& scriptName)
	{
		sol::table loadedScripts = lua["loadedScripts"];
		if (!loadedScripts.valid()) {
			// Initialize loadedScripts table if it doesn't exist
			loadedScripts = lua.create_named_table("loadedScripts");
		}
		return loadedScripts[scriptName].valid();
	}

	LuaValue ScriptHelper::JsonToLuaValue(const JSON& jValue) 
	{
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
	JSON ScriptHelper::LuaValueToJson(const LuaValue& luaValue)
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


	JSON ScriptHelper::GenerateScriptDifferences(const sol::table& script, const std::unordered_map<std::string, LuaValue>& defaults)
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

	void ScriptHelper::SetValue(sol::table& table, const std::string& key, const LuaValue& value)
	{
		std::visit([&table, &key](auto&& arg)
			{
				table[key] = arg;
			}, value);
	}

	LuaValue ScriptHelper::GetValue(const sol::object& value)
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
		else if (value.is<sol::object>()) {
			return  value.as<sol::object>();
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

	void ScriptHelper::ImGuiDisplayAndModifyLuaValue(sol::table& dataOwner, sol::table& table, const std::string& key, LuaValue& value)
	{
		std::visit([&dataOwner, &table, &key, &value](auto&& arg) {
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
			else if constexpr (std::is_same_v<T, sol::table>) {
				sol::table& tableArg = arg;
				NLE_CORE_DEBUG("Table contains: {0} elements", tableArg.empty());
				// Display the table's contents
				for (const auto& pair : arg)
				{
					sol::object key = pair.first;
					sol::object value = pair.second;

					std::string keyStr = key.as<std::string>();
					LuaValue luaValue = ScriptHelper::GetValue(value);

					// Recursively display and modify the table value
					ScriptHelper::ImGuiDisplayAndModifyLuaValue(table, tableArg, keyStr, luaValue);
				}
			}
			else if constexpr (std::is_same_v<T, sol::object>) {
				sol::object objArg = arg;

				// Handle sol::object based on its type
				if (objArg.is<float>()) {
					float v = objArg.as<float>();
					if (ImGui::InputFloat(key.c_str(), &v)) {
						value = v;
						SetValue(dataOwner, key, value);
					}
				}
				else if (objArg.is<int>()) {
					int v = objArg.as<int>();
					if (ImGui::InputInt(key.c_str(), &v)) {
						value = v;
						SetValue(dataOwner, key, value);
					}
				}
				else if (objArg.is<std::string>()) {
					std::string v = objArg.as<std::string>();
					char buffer[256];
					std::strncpy(buffer, v.c_str(), sizeof(buffer));
					buffer[sizeof(buffer) - 1] = 0;
					if (ImGui::InputText(key.c_str(), buffer, sizeof(buffer))) {
						value = std::string(buffer);
						SetValue(dataOwner, key, value);
					}
				}
				else if (objArg.is<bool>()) {
					bool v = objArg.as<bool>();
					if (ImGui::Checkbox(key.c_str(), &v)) {
						value = v;
						SetValue(dataOwner, key, value);
					}
				}
				else if (objArg.is<glm::vec2>()) {
					glm::vec2 v = objArg.as<glm::vec2>();
					if (ImGui::DragFloat2(key.c_str(), glm::value_ptr(v))) {
						value = sol::make_object(dataOwner.lua_state(), v);

						SetValue(dataOwner, key, value);
					}
				}
				else if (objArg.is<glm::vec3>()) {
					glm::vec3 v = objArg.as<glm::vec3>();
					if (ImGui::DragFloat3(key.c_str(), glm::value_ptr(v))) {
						value = sol::make_object(dataOwner.lua_state(), v);

						SetValue(dataOwner, key, value);
					}
				}
				else if (objArg.is<glm::vec4>()) {
					glm::vec4 v = objArg.as<glm::vec4>();
					if (ImGui::DragFloat4(key.c_str(), glm::value_ptr(v))) {
						value = sol::make_object(dataOwner.lua_state(), v);
						SetValue(dataOwner, key, value);
					}
				}

				// Add other types as needed (e.g., sol::function)
			}
			}, value);
	}



}
