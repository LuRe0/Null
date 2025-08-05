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
#include "nlohmann/json.hpp"
#include "../../Tools/ArchetypeDefinitions.h"

using JSON = nlohmann::json;

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
	//using JSON = nlohmann::json;


	class NLE_API Scene;
	class SceneSwitchEvent;

	class NLE_API NSceneManager : public ModuleBase<NSceneManager>
	{
	public:
		NSceneManager();
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
		void RuntimeUpdate(float dt) override;

		//! render function
		void Render();

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

		void SwitchScene(const std::string& nextScene);

		bool LoadSceneFromCache(const std::string& sceneName);

		void ForceReloadSceneDefinition(const std::string& sceneName);

		Scene* GetCurrentScene();



		// Returns pointer to a cached scene definition, or nullptr if not found
		const SceneDefinition* GetSceneDefinition(const std::string& sceneName) const;

		void ClearSceneCache(const std::string& name);

		void CacheSceneDefinition(const std::string& sceneName, const JSON& sceneJson);

		// Checks if a scene definition is already cached
		bool HasSceneDefinition(const std::string& sceneName) const;


		// Loads a scene from its .scene file, parses to SceneDefinition, builds & registers scene
		void LoadSceneFromFile(const std::string& sceneName);


		// Reloads current scene using its cached SceneDefinition
		void ReloadCurrentScene();


		// Writes only the diff between current scene and its archetypes/original data
		void SaveSceneDiff(const std::string& sceneName);

		// Writes full scene state (regardless of archetypes)
		void SaveSceneFull(const std::string& sceneName);



		// Extracts a full definition from the current live Scene (useful for resaving or cloning)
		SceneDefinition GenerateSceneDefinitionFromLiveScene(Scene* scene);


		EntityDefinition ParseEntityDefinition(const JSON& entityJson);

		void RegisterToScripAPI(sol::state& lua) override;


		static const std::string Name() { return "SceneManager"; }

	private:
		//std::vector<std::string> M_CUR = {};
		std::unordered_map<std::string, std::unique_ptr<Scene>> m_Scenes;
		std::string m_CurrentScene = "";
		std::string m_DefaultScene = "";

		std::unordered_map<std::string, SceneDefinition> m_SceneDefinitions;

		void LoadScene(const std::string& scene);

		bool OnSceneSwitch(const SceneSwitchEvent& e);


		NSceneManager(NSceneManager const&) = delete;
		NSceneManager& operator=(NSceneManager const&) = delete;


		// like  static SceneManager = nullptr thats set to this in the constructor?
	};

}