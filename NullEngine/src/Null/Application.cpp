
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
#include "Null/Engine/Submodules/Events/CommonEvents.h"




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
		engine.Add<NULLENGINE::NEventManager>();
		engine.Add<NULLENGINE::NSceneManager>();

		m_NullEngine = &engine;
	}

	void Application::Load()
	{
		m_NullEngine->Load();
	}

	void Application::Init()
	{
		m_NullEngine->Init();
	}

	void Application::Update()
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

	void Application::Unload()
	{
		m_NullEngine->Unload();
	}

	void Application::Shutdown()
	{
		m_NullEngine->Shutdown();
	}

	Application::~Application()
	{
	}

}

