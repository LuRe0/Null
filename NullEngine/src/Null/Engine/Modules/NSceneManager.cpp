
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
//#include "Null/Engine/Submodules/Scene.h"

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
		NEventManager* eventManager =   NEventManager::Instance();

		SUBSCRIBE_EVENT(SceneSwitchEvent, &NSceneManager::OnSceneSwitch, eventManager, EventPriority::Low);

		m_Scenes[m_CurrentScene].get()->Init();
	}


	Scene* NSceneManager::GetCurrentScene()
	{
		return m_Scenes[m_CurrentScene].get();
	}

	const SceneDefinition* NSceneManager::GetSceneDefinition(const std::string& name) const
	{
		auto it = m_SceneDefinitions.find(name);
		return it != m_SceneDefinitions.end() ? &it->second : nullptr;
	}

	void NSceneManager::ClearSceneCache(const std::string& name)
	{
		m_SceneDefinitions.erase(name);
	}

	void NSceneManager::CacheSceneDefinition(const std::string& sceneName, const JSON& sceneJson)
	{
		SceneDefinition def;
		def.originalJson = sceneJson;
		auto* componentFactory = NComponentFactory::Instance();


		for (const auto& entityData : sceneJson["entities"])
		{
			EntityDefinition entDef;
			entDef.referencedArchetype = STRID(entityData.value("archetype", ""));

			if (entityData.contains("components"))
			{
				for (const auto& [compName, compData] : entityData["components"].items())
				{
					const auto& component = componentFactory->CreateComponent(compName + "Component", compData);
					entDef.components[STRID(compName + "Component")] = component;
				}
			}

			if (entityData.contains("children"))
			{
				for (const auto& childDef : entityData["children"])
				{
					std::string baseName = childDef.value("name", "Child");
					std::string uniqueName = def.GenerateUniqueName(baseName);

					entDef.children[STRID(uniqueName)] = ParseEntityDefinition(childDef);
				}
			}

			std::string entityName = entityData.value("name", "");
			def.entities[STRID(def.GenerateUniqueName(entityName))] = std::move(entDef);
		}

		m_SceneDefinitions[sceneName] = std::move(def);
	}

	EntityDefinition NSceneManager::ParseEntityDefinition(const JSON& entityJson)
	{
		auto* componentFactory = NComponentFactory::Instance();

		EntityDefinition entDef;
		entDef.referencedArchetype = STRID(entityJson.value("archetype", ""));

		if (entityJson.contains("components"))
		{
			for (const auto& [compName, compData] : entityJson["components"].items())
			{
				const auto& component = componentFactory->CreateComponent(compName + "Component", compData);
				entDef.components[STRID(compName + "Component")] = component;
			}
		}

		if (entityJson.contains("children"))
		{
			for (const auto& child : entityJson["children"])
			{
				std::string childName = child.value("name", "");
				entDef.children[STRID(childName)] = ParseEntityDefinition(child);
			}
		}

		return entDef;
	}


	void NSceneManager::RegisterToScripAPI(sol::state& lua)
	{
		lua.new_usertype<NSceneManager>(
			"NSceneManager",
			"instantiate", [](NSceneManager* scMan, const std::string& name)
			{
				scMan->GetCurrentScene()->LoadArchetype(name);
			},
			"restart", [](NSceneManager* scMan)
			{
				NEventManager* eventManager =   NEventManager::Instance();


				auto engineState = NEngine::Instance().GetEngineState();

				eventManager->QueueEvent(std::make_unique<EnginePauseStateEvent>(NEngine::PAUSE));

				eventManager->QueueEvent(std::make_unique<SceneSwitchEvent>(scMan->GetCurrentScene()->GetName(), scMan->GetCurrentScene()->GetName()));

				eventManager->QueueEvent(std::make_unique<EngineRunStateEvent>(engineState));

			},
			"load", [](NSceneManager* scMan, const std::string& name)
			{

				NEventManager* eventManager =   NEventManager::Instance();

				auto engineState = NEngine::Instance().GetEngineState();

				eventManager->QueueEvent(std::make_unique<EnginePauseStateEvent>(NEngine::PAUSE));

				eventManager->QueueEvent(std::make_unique<SceneSwitchEvent>(scMan->GetCurrentScene()->GetName(), name));

				eventManager->QueueEvent(std::make_unique<EngineRunStateEvent>(engineState));

			}
		);

		// Expose the existing instance to Lua under a different global variable
		lua["Scene"] = this;

		lua.set_function("Instantiate", sol::overload(
			[this](const std::string& name, sol::this_state s) -> sol::object {
				EntityID id = this->GetCurrentScene()->LoadArchetype(name);
				Entity& entity = this->GetCurrentScene()->GetEntity(id);
				return sol::make_reference(s, std::ref(entity));
			},
			[this](const std::string& name, float lifetime, sol::this_state s) -> sol::object {
				EntityID id = this->GetCurrentScene()->LoadArchetype(name);
				Entity& entity = this->GetCurrentScene()->GetEntity(id);
				entity.Add<LifetimeComponent>(lifetime);
				return sol::make_reference(s, std::ref(entity));
			}
		));

	}

	void NSceneManager::LoadScene(const std::string& scene)
	{
		NEventManager* eventManager =   NEventManager::Instance();

		eventManager->QueueEvent(std::make_unique<SceneSwitchEvent>(m_CurrentScene, scene));
	}

	bool NSceneManager::OnSceneSwitch(const SceneSwitchEvent& e)
	{
		m_Scenes.clear();

		SwitchScene(e.GetNextScene());

		return true;
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
		for( auto& def : m_SceneDefinitions)
		{
			def.second.Destroy();
		}

		m_SceneDefinitions.clear();
	}

	void NSceneManager::Shutdown()
	{
	}

	void NSceneManager::SwitchScene(const std::string& nextScene)
	{
		// Try cache first
		if (LoadSceneFromCache(nextScene)) {
			NLE_CORE_INFO("Loaded scene '{0}' from cache", nextScene);
			return;
		}

		std::string filePath = "../Assets/Scenes/Paths/" + nextScene + ".scene";
		std::ifstream inputFile(filePath);

		if (!inputFile.is_open())
		{
			NLE_CORE_ERROR("Could not open file: {0}", filePath);

			if (m_Parent->GetIsEditorEnabled())
			{
				std::unique_ptr<Scene> newScene = std::make_unique<Scene>("New Scene");
				RegisterScene("New Scene", std::move(newScene));
				m_CurrentScene = "New Scene";
				NLE_CORE_WARN("Created empty scene");
			}

			return;
		}

		JSON sceneData;
		try {
			inputFile >> sceneData;
		}
		catch (JSON::parse_error& e) {
			NLE_ERROR("JSON parsing failed: {0}", e.what());
			return;
		}

		// Cache it
		CacheSceneDefinition(nextScene, sceneData);

		std::unique_ptr<Scene> scene = std::make_unique<Scene>(nextScene);
		scene->BuildFromDefinition(m_SceneDefinitions[nextScene]);
		RegisterScene(nextScene, std::move(scene));

		m_CurrentScene = nextScene;


		//exit(0); // Exit the program after building the scene from definition

	}



	bool NSceneManager::LoadSceneFromCache(const std::string& sceneName)
	{
		auto it = m_SceneDefinitions.find(sceneName);
		if (it == m_SceneDefinitions.end())
			return false;

		std::unique_ptr<Scene> scene = std::make_unique<Scene>(sceneName);
		scene->BuildFromDefinition(it->second);
		RegisterScene(sceneName, std::move(scene));
		m_CurrentScene = sceneName;

		return true;
	}

	void NSceneManager::ForceReloadSceneDefinition(const std::string& sceneName)
	{
		std::string filePath = "../Assets/Scenes/Paths/" + sceneName + ".scene";
		std::ifstream inputFile(filePath);

		if (!inputFile.is_open())
		{
			NLE_CORE_ERROR("Could not reopen scene for reload: {0}", filePath);
			return;
		}

		JSON sceneData;
		try {
			inputFile >> sceneData;
		}
		catch (JSON::parse_error& e) {
			NLE_CORE_ERROR("Failed to re-parse scene: {0}", e.what());
			return;
		}

		CacheSceneDefinition(sceneName, sceneData);
	}

}