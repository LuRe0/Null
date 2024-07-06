
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

using JSON = nlohmann::json;

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

void NULLENGINE::NSceneManager::Load()
{
    std::string filePath = std::string("../Data/Scenes/") + std::string("ScenePaths") + std::string(".json");

    // Open the JSON file
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        NLE_ERROR("Error: Could not open file");
        return;
    }

    // Parse the JSON file
    JSON jsonData;
    try {
        inputFile >> jsonData;
    }
    catch (JSON::parse_error& e) {
        NLE_ERROR("Error: JSON parsing failed: _{0}", e.what());
        return ;
    }

    //Read the list of scenes in this project
    for (const auto& item : jsonData.at("Scenes")) {
        m_SceneNames.push_back(item.at("name"));
    }
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
