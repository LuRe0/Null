#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	SpriteRenderSystem.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Submodules/ECS/Systems/System.h"


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
	class NLE_API SpriteRenderSystem : public ISystem
	{
	public:

		SpriteRenderSystem()
		{
			Require<TransformComponent>();
			Require<SpriteComponent>();
		}
		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;

		void Render() const override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

	private:

	};

}
