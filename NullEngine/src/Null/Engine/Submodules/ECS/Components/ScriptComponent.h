#pragma once
#include "IComponent.h"
#include "sol/sol.hpp"

namespace NULLENGINE
{
	using LuaValue = std::variant<
		float,
		int,
		std::string,
		bool,
		sol::object,
		sol::table,
		sol::function,
		std::monostate
	>;

	struct OrderedMap {
		std::vector<std::pair<std::string, LuaValue>> entries = std::vector<std::pair<std::string, LuaValue>>();
		std::unordered_map<std::string, size_t> indexMap = std::unordered_map<std::string, size_t>();

		LuaValue& operator[](const std::string& key) {
			auto it = indexMap.find(key);
			if (it != indexMap.end()) {
				// Return existing value
				return entries[it->second].second;
			}
			// Insert new entry with default LuaValue
			entries.emplace_back(key, LuaValue{});
			indexMap[key] = entries.size() - 1;
			return entries.back().second;
		}

		LuaValue& at(const std::string& key) 
		{
			auto it = indexMap.find(key);
			if (it == indexMap.end()) {
				NLE_CORE_THROW("OrderedMap::at: key not found: {}",  key);
			}
			return entries[it->second].second;
		}

		const LuaValue& at(const std::string& key) const {
			auto it = indexMap.find(key);
			if (it == indexMap.end()) 
			{
				NLE_CORE_THROW("OrderedMap::at: key not found: {}", key);
			}
			return entries[it->second].second;
		}

		bool contains(const std::string& key) const {
			return indexMap.find(key) != indexMap.end();
		}

		const std::unordered_map<std::string, LuaValue> toMap() const
		{
			std::unordered_map<std::string, LuaValue> map;
			for (const auto& [key, val] : entries) {
				map.emplace(key, val);
			}
			return map;
		}
	};

	using ScriptDefaults = std::unordered_map<std::string, OrderedMap>;

	//bool operator==(const LuaValue& lhs, const LuaValue& rhs) {
	//	return lhs.index() == rhs.index() && std::visit(
	//		[](const auto& lhs_value, const auto& rhs_value) -> bool {
	//			return lhs_value == rhs_value;
	//		},
	//		lhs, rhs
	//	);
	//}

	//bool operator!=(const LuaValue& lhs, const LuaValue& rhs) {
	//	return !(lhs == rhs);
	//}
	struct ScriptComponent : public BaseComponent
	{


		sol::environment m_Environment;
		std::vector<sol::table> m_Scripts;
		std::vector<std::string> m_Script_Names;
		std::vector<std::string> m_Script_Paths;

		//std::map<std::string, LuaValue> m_Script

		ScriptDefaults m_ScriptDefaults;

        ScriptComponent(const std::vector<std::string>& script_names = std::vector<std::string>(), ScriptDefaults defaults = ScriptDefaults())
			: m_Script_Names(script_names), m_ScriptDefaults(defaults)
        {

        }


		ScriptComponent& operator=(const ScriptComponent & other)
		{
			m_Script_Names = other.m_Script_Names;
			m_ScriptDefaults = other.m_ScriptDefaults;

			return *this;
		}

		const std::string Name() const { return Component<ScriptComponent>::TypeName(); };
		const uint32_t ID() const { return Component<ScriptComponent>::GetID(); };

	};
}