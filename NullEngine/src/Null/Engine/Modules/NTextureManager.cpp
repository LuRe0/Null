
//------------------------------------------------------------------------------
//
// File Name:	NTextureManager.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NTextureManager.h"
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
	void NTextureManager::Load()
	{
		std::string filePath = std::string("../Data/Textures");

		for (const auto& entry : std::filesystem::directory_iterator(filePath))
		{
			if (entry.is_regular_file()) // Ensure it's a regular file (not a directory or symlink)
			{
				const std::string& filePath = entry.path().string();

				size_t lastSlash = filePath.find_last_of("\\/");
				size_t start = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;

				size_t lastDot = filePath.find_last_of('.');
				size_t end = (lastDot == std::string::npos) ? filePath.length() : lastDot;

				const std::string& name = filePath.substr(start, end - start);

				Create(name);
			}
		}
	}

	void NTextureManager::Unload()
	{
		NResourceManager::Unload();
	}

}