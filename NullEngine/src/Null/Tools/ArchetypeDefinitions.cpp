
//------------------------------------------------------------------------------
//
// File Name:	NStub.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "ArchetypeDefinitions.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void SceneDefinition::Destroy()
	{
		for (auto& [entityName, entDef] : entities)
		{
			entDef.Destroy();
		}
		entities.clear();
		//originalJson.clear();
	}
	
	std::string SceneDefinition::GenerateUniqueName(const std::string& baseName)
	{
		int& count = nameFrequency[STRID(baseName)];
		std::string newName = (count == 0) ? baseName : baseName + "_" + std::to_string(count);
		count++;
		return newName;
	}


	void EntityDefinition::Destroy()
	{
		// No manual delete needed anymore, just clear blobs
		components.clear();

		for (auto& [childName, childDef] : children)
		{
			childDef.Destroy(); // Recursively destroy children
		}
		children.clear();
	}
}
