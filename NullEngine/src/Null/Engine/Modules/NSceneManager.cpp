
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

using JSON = nlohmann::json;

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void NSceneManager::RegisterScene(const std::string& name, std::unique_ptr<Scene>&& scene)
	{
		m_Scenes[name] = std::move(scene);

		NLE_CORE_TRACE("{0} scene addded successfully added to list", name);
	}

	void NSceneManager::Load()
	{
		std::string filePath = std::string("Data/Scenes/") + std::string("TestScene") + std::string(".json");

		// Open the JSON file
		std::ifstream inputFile(filePath);
		if (!inputFile.is_open()) {
			NLE_ERROR("Error: Could not open file");
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


		std::string sceneName = sceneData["sceneName"];

		std::unique_ptr<Scene> scene = std::make_unique<Scene>(sceneName);

		scene->Load(sceneName, sceneData);

		RegisterScene(sceneName, std::move(scene));

		m_CurrentScene = sceneName;
		//ChangeScene(sceneName);
	}

	void NSceneManager::Init()
	{
		m_Scenes[m_CurrentScene].get()->Init();
	}


	Scene* NSceneManager::GetCurrentScene()
	{
		return m_Scenes[m_CurrentScene].get();
	}

	void NSceneManager::Update(float dt)
	{
		//m_Scenes[m_CurrentScene].get()->Update();
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
}