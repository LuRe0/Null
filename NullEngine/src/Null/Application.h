#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Application.h
// Author(s):	Anthon Reid 
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Core.h"
#include "Null/Engine/NEngine.h"


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //

namespace NULLENGINE
{
	class NLE_API Application
	{
	public:
		Application();

		virtual void Load();
		//! Virtual Init function
		virtual void Init();
		//! Virtual Update function
		virtual void Update();

		virtual void Unload();
		//! Virtual Shutdown function
		virtual void Shutdown();

		virtual ~Application();

		virtual void Run();
	private:
		IEngine* m_NullEngine;
	};

	//To be defineded by user

	Application* CreateApplication();
}