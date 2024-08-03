#pragma once
#include "IComponent.h"
#include "sol/sol.hpp"

namespace NULLENGINE
{

	struct ScriptComponent : public BaseComponent
	{
		sol::state m_Lua_state;
		std::vector<sol::table> m_Scripts;
		std::vector<std::string> m_Script_Names;

        ScriptComponent(const std::vector<std::string>& script_names = std::vector<std::string>()) : m_Script_Names(script_names)
        {

        }


		ScriptComponent& operator=(const ScriptComponent & other)
		{
			m_Script_Names = other.m_Script_Names;

			return *this;
		}

		const std::string Name() { return Component<ScriptComponent>::TypeName(); };
	};
}