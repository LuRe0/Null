
//------------------------------------------------------------------------------
//
// File Name:	NSceneManager.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NSceneManager.h"
#include "Null/Tools/Trace.h"
#include <nlohmann/json.hpp>
#include "Null/Engine/Submodules/Scene.h"
#include "Null/Engine/Submodules/Events/IEvents.h"

using JSON = nlohmann::json;

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	NSceneManager::NSceneManager() : m_DefaultScene("")
	{
		std::string filePath = std::string("../Assets/Scenes/") + std::string("Init") + std::string(".json");

		// Open the JSON file
		std::ifstream inputFile(filePath);
		if (!inputFile.is_open())
		{
			NLE_CORE_ERROR("Error: Could not open file");
			return;
		}

		// Parse the JSON file
		JSON sceneData;

		try {
			inputFile >> sceneData;
		}
		catch (JSON::parse_error& e) {
			NLE_ERROR("Error: JSON parsing failed: _{0}", e.what());
			return;
		}


		std::string startScene = sceneData["startScene"];

		m_DefaultScene = startScene;
	}
	void NSceneManager::RegisterScene(const std::string& name, std::unique_ptr<Scene>&& scene)
	{
		m_Scenes[name] = std::move(scene);

		NLE_CORE_TRACE("{0} scene addded successfully added to list", name);
	}

	void NSceneManager::Load()
	{
		SwitchScene(m_DefaultScene);
		//ChangeScene(sceneName);
	}

	void NSceneManager::Init()
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		SUBSCRIBE_EVENT(SceneSwitchEvent, &NSceneManager::OnSceneSwitch, eventManager, EventPriority::Low);

		m_Scenes[m_CurrentScene].get()->Init();
	}


	Scene* NSceneManager::GetCurrentScene()
	{
		return m_Scenes[m_CurrentScene].get();
	}

	void NSceneManager::RegisterToScripAPI(sol::state& lua)
	{
		lua.new_usertype<NSceneManager>(
			"NSceneManager",
			"CurrentScene", &NSceneManager::GetCurrentScene,
			"LoadScene", &NSceneManager::LoadScene
		);

		// Expose the existing instance to Lua under a different global variable
		lua["SceneManager"] = this;
	}

	void NSceneManager::LoadScene(const std::string& scene)
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		eventManager->QueueEvent(std::make_unique<SceneSwitchEvent>(m_CurrentScene, scene));
	}

	void NSceneManager::OnSceneSwitch(const SceneSwitchEvent& e)
	{
		m_Scenes.clear();

		SwitchScene(e.GetNextScene());
	}

	void NSceneManager::Update(float dt)
	{
		m_Scenes[m_CurrentScene].get()->Update(dt);
	}

	void NSceneManager::RuntimeUpdate(float dt)
	{
		Update(dt);
	}

	void NSceneManager::Render()
	{
		m_Scenes[m_CurrentScene].get()->Render();
	}

	void NSceneManager::Unload()
	{
	}

	void NSceneManager::Shutdown()
	{
	}

	void NSceneManager::SwitchScene(const std::string& nextScene)
	{
		std::string filePath = std::string("../Assets/Scenes/Paths/") + nextScene + std::string(".scene");

		// Open the JSON file
		std::ifstream inputFile(filePath);
		if (!inputFile.is_open())
		{
			NLE_CORE_ERROR("Error: Could not open file");

			if (m_Parent->GetIsEditorEnabled())
			{
				std::unique_ptr<Scene> scene = std::make_unique<Scene>("New Scene");

				RegisterScene("New Scene", std::move(scene));

				m_CurrentScene = "New Scene";

				NLE_CORE_WARN("Creating Empty Scene");
			}

			return;
		}

		// Parse the JSON file
		JSON sceneData;

		try {
			inputFile >> sceneData;
		}
		catch (JSON::parse_error& e) {
			NLE_ERROR("Error: JSON parsing failed: _{0}", e.what());
			return;
		}


		std::string sceneName = nextScene;

		std::unique_ptr<Scene> scene = std::make_unique<Scene>(sceneName);

		scene->Load(sceneName, sceneData);

		RegisterScene(sceneName, std::move(scene));

		m_CurrentScene = sceneName;
	}
}