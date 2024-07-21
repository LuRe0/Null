#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NSpriteSourceManager.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include "Null/Engine/Modules/NResourceManager.h"
#include "Null/Engine/Submodules/Graphics/SpriteSource.h"


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

	// Specialize for resource
	class NLE_API NSpriteSourceManager : public NResourceManager<SpriteSource>
	{
	public:


		void Load() override {};

		//! Virtual Init function
		void Init() override {};

		//! Virtual Update function
		void Update(float dt) override {};

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override {};
	};

}