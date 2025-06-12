
//------------------------------------------------------------------------------
//
// File Name:	NTilesetManager.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NTilesetManager.h"
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
	void NTilesetManager::Load()
	{
		std::string folderPath = "../Assets/Tilesets";

		for (const auto& entry : std::filesystem::directory_iterator(folderPath))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".tsj")
			{
				std::string fullPath = entry.path().string();
				std::string filename = entry.path().filename().string();

				size_t dotPos = filename.find_last_of('.');
				std::string name = (dotPos != std::string::npos) ? filename.substr(0, dotPos) : filename;

				auto tileset = Create(name);
				//tileset->Load(); // Optional if Load() does more later
			}
		}
	}

	void NTilesetManager::Unload()
	{
		NResourceManager::Unload();
	}

}