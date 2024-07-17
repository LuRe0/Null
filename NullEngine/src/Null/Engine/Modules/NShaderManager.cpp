
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
		std::string filePath = std::string("Data/Shaders/") + std::string("paths") + std::string(".json");

		// Open the JSON file
		std::ifstream inputFile(filePath);
		if (!inputFile.is_open()) {
			NLE_ERROR("Error: Could not open file");
			return;
		}

		// Parse the JSON file
		JSON shaderData;

		try {
			inputFile >> shaderData;
		}
		catch (JSON::parse_error& e) {
			NLE_ERROR("Error: JSON parsing failed: _{0}", e.what());
			return;
		}

		for (const auto& paths : shaderData["shaders"])
		{
			const std::string path = paths["path"];

			CreateShader(path);
		}
	}

	void NShaderManager::Init()
	{
		
	}

	void NShaderManager::Update(float dt)
	{

	}

	void NShaderManager::Unload()
	{
	}

	void NShaderManager::Shutdown()
	{
	}
	void NShaderManager::AddShader(const std::string& name, Shader* shader)
	{
		NLE_CORE_ASSERT(!mShaderList.contains(name), "Shader {0} already exists", name);


		mShaderList[name] = shader;
	}

	Shader* NShaderManager::GetShader(const std::string& name)
	{
		NLE_CORE_ASSERT(mShaderList.contains(name), "Shader {0} does not exist", name);


		return mShaderList.at(name);
	}

	Shader* NShaderManager::CreateShader(const std::string& name)
	{
		if (!mShaderList.contains(name))
		{
			Shader* shader = new Shader(name, name);

			AddShader(name, shader);

			return shader;
		}


		return mShaderList.at(name);
	}
}