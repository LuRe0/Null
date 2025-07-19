#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NWindow.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"


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
	class NLE_API NStub : public ModuleBase<NStub>
	{
	public:


		void Load() override;
		void Init() override;
		void Update(float dt) override;
		void RuntimeUpdate(float dt) override;
		//void Render() override;
		void RenderLoadScreen();
		void Unload() override;
		void Shutdown() override;

		bool HasRenderImGui() const override { return true; }

		void RegisterToScripAPI(sol::state& lua) override;

		void RenderImGui() override;

	private:

	};

}