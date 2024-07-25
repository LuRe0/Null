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
#include "Null/Engine/Submodules/Scene.h"


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

	class NLE_API NSceneManager : public IModule
	{
	public:
		NSceneManager() = default;
		~NSceneManager() = default;

		/// <summary>
		/// Adds a scene to the list of currently loaded in scene
		/// </summary>
		/// <param name="name"></param>
		/// <param name="scene"></param>
		void RegisterScene(const std::string& name, std::unique_ptr<Scene>&& scene);

		/// <summary>
		/// load in scene information
		/// </summary>
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

		Scene* GetCurrentScene();

		static const std::string Name() { return "SceneManager"; }

	private:
		//std::vector<std::string> M_CUR = {};
		std::unordered_map<std::string, std::unique_ptr<Scene>> m_Scenes;
		std::string m_CurrentScene = "";

		NSceneManager(NSceneManager const&) = delete;
		NSceneManager& operator=(NSceneManager const&) = delete;

	};

}