
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
#include "Null/Engine/Submodules/ECS/Components/IComponent.h"
#include "Null/Engine/Modules/NEventManager.h"
#include "glad//glad.h"




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
		engine.Add<NULLENGINE::NRegistry>();
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

		NWindow* window = m_NullEngine->Get<NWindow>();

		window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
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



			for (auto& layer : m_layers)
				layer.second.get()->OnUpdate(dt);


			glfwSwapBuffers(window->GetWinddow());

		
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

	void Application::PushOverlay(std::unique_ptr<ILayer>&& overlay)
	{
		overlay.get()->OnAttach();

		m_layers.emplace(std::make_pair(OVERLAY, std::move(overlay)));
	}

	void Application::PushLayer(std::unique_ptr<ILayer>&& layer)
	{
		layer.get()->OnAttach();
		m_layers.emplace(std::make_pair(LAYER, std::move(layer)));
	}

	void Application::OnEvent(const Event& e)
	{
		NLE_CORE_INFO("{0}", e.Print());


		for (auto& layer : m_layers)
			layer.second.get()->OnEvent(e);

	}


	Application::~Application()
	{
	}

}

