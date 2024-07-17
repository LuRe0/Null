#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	TransformSystem.h
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
	class NLE_API TransformSystem : public ISystem
	{
	public:

		TransformSystem()
		{
			Require<TransformComponent>();
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
		//void OnWindowResize(const WindowResizeEvent& e);
	};

}
