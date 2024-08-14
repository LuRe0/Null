#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NDebugManager.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
//#include "Null/Engine/Submodules/Graphics/Camera/Camera.h"
#include "Null/Engine/Submodules/Graphics/Camera/Camera2D.h"
#include "Null/Engine/Submodules/Graphics/Camera/Camera3D.h"


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
	struct NLE_API NDebugManager : public IModule
	{
	public:
        NDebugManager() = default;
		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;
        void RuntimeUpdate(float dt) override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

        void RegisterToScripAPI(sol::state& lua) override {};

	public:
		bool m_ShowDebug = false;
		//bool m_ShowKinematics = false;

	private:

        NDebugManager(NDebugManager const&);
        NDebugManager& operator=(NDebugManager const&);
	};


}