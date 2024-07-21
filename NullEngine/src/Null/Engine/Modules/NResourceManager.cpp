
//------------------------------------------------------------------------------
//
// File Name:	NResourceManager.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NResourceManager.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	//void NResourceManager::Load()
	//{
	//	std::string filePath = std::string("Data/Shaders/") + std::string("paths") + std::string(".json");

	//	// Open the JSON file
	//	std::ifstream inputFile(filePath);
	//	if (!inputFile.is_open()) {
	//		NLE_ERROR("Error: Could not open file");
	//		return;
	//	}

	//	// Parse the JSON file
	//	JSON shaderData;

	//	try {
	//		inputFile >> shaderData;
	//	}
	//	catch (JSON::parse_error& e) {
	//		NLE_ERROR("Error: JSON parsing failed: _{0}", e.what());
	//		return;
	//	}

	//	for (const auto& paths : shaderData["shaders"])
	//	{
	//		const std::string path = paths["path"];

	//		CreateShader(path);
	//	}
	//}



}