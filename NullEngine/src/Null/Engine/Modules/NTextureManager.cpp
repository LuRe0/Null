
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
		std::string filePath = std::string("../Assets/Textures");

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

				auto tex = Create(name);
				tex->Init();
				GetTextureIndex(name);
			}
		}
	}

	void NTextureManager::Unload()
	{
		NResourceManager::Unload();

		for (auto& tex : m_ResourceList)
			tex.second->Shutdown();
	}

	void NTextureManager::AddTextureIndex(const std::string& textureName)
	{
		// If textureName not in map, assign next available index
		if (m_TextureSlots.find(textureName) == m_TextureSlots.end())
		{
			m_TextureSlots[textureName] = CreateIndex();
		}
	}

	uint32_t NTextureManager::GetTextureIndex(const std::string& textureName)
	{
		auto it = m_TextureSlots.find(textureName);
		if (it != m_TextureSlots.end())
			return it->second;

		// If not found, add it:
		AddTextureIndex(textureName);
		return m_TextureSlots[textureName];
	}

}