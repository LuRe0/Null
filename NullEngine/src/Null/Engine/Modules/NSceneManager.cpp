
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

void NULLENGINE::NSceneManager::RegisterScene(const std::string& name, std::shared_ptr<Scene> scene)
{
    m_Scenes[name] = scene;

    NLE_CORE_TRACE("{0} scene addded successfully added to list", name);
}

void NULLENGINE::NSceneManager::Load()
{
    std::string filePath = std::string("../Data/Scenes/") + std::string("TestScene") + std::string(".json");

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
        return ;
    }


    std::string sceneName = sceneData["sceneName"];
    auto scene = std::make_shared<Scene>();


    RegisterScene(sceneName, scene);

    scene->Load(sceneName,sceneData);
    //ChangeScene(sceneName);
}

void NULLENGINE::NSceneManager::Init()
{
}

void NULLENGINE::NSceneManager::Update(float dt)
{
}

void NULLENGINE::NSceneManager::Render()
{
}

void NULLENGINE::NSceneManager::Unload()
{
}

void NULLENGINE::NSceneManager::Shutdown()
{
}
