#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	IModule.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"




//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//
namespace NULLENGINE
{

	struct BaseComponent;

	struct EntityDefinition
	{
		uint32_t referencedArchetype; // empty if not a reference
		std::unordered_map<uint32_t, std::vector<uint8_t>> components; // component binary blobs
		std::unordered_map<uint32_t, EntityDefinition> children;

		void Destroy();

	};


	typedef std::unordered_map<uint32_t, EntityDefinition> ArchetypeContainer;


	struct SceneDefinition
	{
		ArchetypeContainer entities; // Same as: std::unordered_map<std::string, EntityDefinition>
		nlohmann::json originalJson; // Optional: for diff save
		std::unordered_map<uint32_t, int> nameFrequency;

		std::string GenerateUniqueName(const std::string& baseName);

		void Destroy();
	
	};
}