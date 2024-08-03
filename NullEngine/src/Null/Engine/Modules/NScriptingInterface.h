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
		void Update(float dt) override {};

		//! render function
		void Render() {};

		void Unload() override {};
		//! Virtual Shutdown function
		void Shutdown() override {};

		void RegisterEngineFunctions(sol::state& lua);

	/*	
		void Render(const RenderData& render);*/

		void RegisterToScripAPI(sol::state& lua) override {};

	private:

		NScriptingInterface(NScriptingInterface const&) = delete;
		NScriptingInterface& operator=(NScriptingInterface const&) = delete;

		void RegisterMathStructures(sol::state& lua);
		void RegisterEntity(sol::state& lua);
	};

}