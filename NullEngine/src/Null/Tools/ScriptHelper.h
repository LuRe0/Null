#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	ScriptHelper.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"


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
	class NLE_API ScriptHelper
	{
	public:

		static LuaValue GetValue(const sol::object& value);
		static bool LoadScript(sol::state& lua, const std::string& scriptName);
		static void SetValue(sol::table& table, const std::string& key, const LuaValue& value);
		static void ImGuiDisplayAndModifyLuaValue(sol::table& dataOwner, sol::table& table, const std::string& key, LuaValue& value);
		static JSON GenerateScriptDifferences(const sol::table& script, const std::unordered_map<std::string, LuaValue>& defaults);
		static JSON LuaValueToJson(const LuaValue& luaValue);
		static LuaValue JsonToLuaValue(const JSON& jValue);
		static bool IsScriptLoaded(sol::state& lua, const std::string& scriptName);
		static void MarkScriptAsLoaded(sol::state& lua, const std::string& scriptName);

	private:

	};

}