
//------------------------------------------------------------------------------
//
// File Name:	NShaderManager.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NShaderManager.h"
#include "Null/Engine/Submodules/ECS/Systems/PhysicsSystem.h"
#include "Null/Engine/Submodules/Graphics/Shader/Shader.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void NShaderManager::Load()
	{
		std::string filePath = std::string("../Data/Shaders/") + std::string("paths") + std::string(".json");

		// Open the JSON file
		std::ifstream inputFile(filePath);
		if (!inputFile.is_open()) {
			NLE_ERROR("Error: Could not open file");
			return;
		}

		// Parse the JSON file
		JSON resourceData;

		try {
			inputFile >> resourceData;
		}
		catch (JSON::parse_error& e) {
			NLE_ERROR("Error: JSON parsing failed: _{0}", e.what());
			return;
		}

		for (const auto& paths : resourceData["shaders"])
		{
			const std::string path = paths["path"];

			Create(path);
		}
	}

	void NShaderManager::Unload()
	{
		NResourceManager::Unload();
	}

}