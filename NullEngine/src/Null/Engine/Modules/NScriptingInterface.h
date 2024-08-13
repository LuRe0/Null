#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NScriptingInterface.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include "sol/sol.hpp"
#include <FileWatch.hpp>


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//

using Watcher = std::vector<std::unique_ptr<filewatch::FileWatch<std::wstring>>>;

//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//


namespace NULLENGINE
{


	//class NLE_API Scene;

	class NLE_API NScriptingInterface : public IModule
	{
	public:
		NScriptingInterface() = default;
		~NScriptingInterface() = default;

		/// <summary>
		/// load in scene information
		/// </summary>
		void Load() override;


		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;
		void RuntimeUpdate(float dt) override;

		//! render function
		void Render() {};

		void Unload() override {};
		//! Virtual Shutdown function
		void Shutdown() override {};

		void RegisterEngineFunctions(sol::state& lua);

		void CreateScript(const std::string& filename);

		void ReloadScript(const std::string& filename);

		void AddScriptWatcher(const std::string filepath, const std::string& filename);

	/*	
		void Render(const RenderData& render);*/

		void AddScripts(const std::string& filename);

		void UpdateScript(const std::string& filename, bool m);

		bool GetScriptStatus(const std::string& filename);

		void RegisterToScripAPI(sol::state& lua) override {};


		std::vector<std::string> GetScriptNames() const
		{
			std::vector<std::string> componentNames;
			for (const auto& pair : m_ScriptList) {
				componentNames.push_back(pair.first);
			}
			return componentNames;
		}


	private:
		Watcher m_ScriptReloaders;
		std::unordered_map<std::string, bool> m_ScriptList;

		NScriptingInterface(NScriptingInterface const&) = delete;
		NScriptingInterface& operator=(NScriptingInterface const&) = delete;

		 void RegisterMathStructures(sol::state& lua);
		 void RegisterVec2Structures(sol::state& lua);
		 void RegisterVec3Structures(sol::state& lua);
		 void RegisterVec4Structures(sol::state& lua);
		 void MathFreeFunctions(sol::state& lua);
	};

}