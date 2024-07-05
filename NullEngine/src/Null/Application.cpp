
//------------------------------------------------------------------------------
//
// File Name:	Application.cpp
// Author(s):	Anthon Reid 
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Application.h"
#include "NIncludes.h"



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	Application::Application()
	{
		IEngine& engine = NEngine::Instance();
		
		engine.Add<NULLENGINE::NWindow>();

		m_NullEngine = &engine;
	}

	void Application::Load()
	{
	}

	void Application::Init()
	{
		m_NullEngine->Init();
	}

	void Application::Update()
	{
	}

	void Application::Unload()
	{
	}

	void Application::Shutdown()
	{
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		 NWindow* window = m_NullEngine->Get<NWindow>();

		 if (!window)
			 return;

		Time& time = Time::Instance();



		while (!window->WindowClosed())
		{
			time.Update();

			float dt = time.DeltaTime();

			m_NullEngine->Update(dt);
			//NLE_TRACE(__cplusplus);
		}
	}
}

