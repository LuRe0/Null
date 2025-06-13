
//------------------------------------------------------------------------------
//
// File Name:	NComputeShaderManager.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NComputeShaderManager.h"
#include "Null/Engine/Submodules/ECS/Systems/PhysicsSystem.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void NComputeShaderManager::Load()
	{
		std::string filePath = std::string("../Assets/Shaders/") + std::string("paths") + std::string(".json");

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

			if (paths.contains("isCompute"))
			{
				bool compute = paths["isCompute"];

				if (compute)
					Create(path);
			}
		}
	}

	void NComputeShaderManager::Unload()
	{
		NResourceManager::Unload();
	}

}