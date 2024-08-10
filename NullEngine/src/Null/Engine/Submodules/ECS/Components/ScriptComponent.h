#pragma once
#include "IComponent.h"
#include "sol/sol.hpp"

namespace NULLENGINE
{
	using LuaValue = std::variant<float, int, std::string, bool, sol::table, sol::function, std::monostate>;
	using ScriptDefaults = std::unordered_map<std::string, std::unordered_map<std::string, LuaValue>>;

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

			return *this;
		}

		const std::string Name() const { return Component<ScriptComponent>::TypeName(); };
		const uint32_t ID() const { return Component<ScriptComponent>::GetID(); };

	};
}