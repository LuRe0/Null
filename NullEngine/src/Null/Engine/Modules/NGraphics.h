#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NGraphics.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include <Null/Engine/Submodules/Graphics/Buffers/RenderData.h>



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

	//class NLE_API Scene;

	class NLE_API NGraphics : public IModule
	{
	public:
		NGraphics() = default;
		~NGraphics() = default;

		/// <summary>
		/// load in scene information
		/// </summary>
		void Load() override {};


		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override {};

		//! render function
		void Render() {};

		void Unload() override {};
		//! Virtual Shutdown function
		void Shutdown() override {};


	/*	
		void Render(const RenderData& render);*/

	private:



		NGraphics(NGraphics const&) = delete;
		NGraphics& operator=(NGraphics const&) = delete;

	};

}