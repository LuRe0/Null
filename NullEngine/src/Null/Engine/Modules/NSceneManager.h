#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NSceneManager.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"


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
	class NLE_API Scene;

	class NLE_API NSceneManager : public IModule
	{
	public:
		NSceneManager() = default;
		~NSceneManager() = default;

		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;

		//! render function
		void Render();

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

		static const std::string Name() { return "SceneManager"; }

	private:
		std::vector<std::string> m_SceneNames = {};
		std::unordered_map<std::string, Scene*> m_Scenes;
		std::string m_CurrentScene = "";
	};

}