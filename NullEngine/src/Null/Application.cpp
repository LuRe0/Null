
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
#include "Null/Engine/Submodules/Layers/ImGuiLayer.h"
#include "Null/Engine/Submodules/Graphics/Buffers/VAO.h"
#include "Null/Engine/Submodules/Graphics/Buffers/VBO.h"
#include "Null/Engine/Submodules/Graphics/Buffers/EBO.h"
#include "Null/Engine/Submodules/Graphics/Texture.h"
#include "Null/Engine/Submodules/Graphics/Mesh.h"




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
		engine.Add<NULLENGINE::NCameraManager>();
		engine.Add<NULLENGINE::NRegistry>();
		engine.Add<NULLENGINE::NShaderManager>();
		engine.Add<NULLENGINE::NSceneManager>();
		engine.Add<NULLENGINE::NRenderer>();

		m_NullEngine = &engine;


		//vertex array
		//vertex buffer
		//index buffer
	}

	void Application::Load()
	{
		m_NullEngine->Load();

		m_ImGuiLayer = std::make_unique<NULLENGINE::ImGuiLayer>();
		m_ImGuiLayer->OnAttach();
	}

	void Application::Init()
	{
		Input::Instance().Init();
		m_NullEngine->Init();

		NWindow* window = m_NullEngine->Get<NWindow>();

		window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	}

	void Application::Update()
	{
		NWindow* window = m_NullEngine->Get<NWindow>();

		if (!window)
			return;



		// Create VBO
		/*VAO vao;
		vao.Bind();
		VBO vbo(vertices);
		EBO ebo(indeces);
		vao.Attach();
		vao.Unbind();*/

		while (!window->WindowClosed() && !window->WindowMinimized())
		{

			Input::Update();

			Time::Update();

			float dt = Time::DeltaTime();

			m_NullEngine->Update(dt);


			m_ImGuiLayer->Begin();
			m_ImGuiLayer->OnRender();
			m_ImGuiLayer->End();


			glfwSwapBuffers(window->GetWinddow());

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
		//overlay.get()->OnAttach();

		//m_layers.emplace(std::make_pair(OVERLAY, std::move(overlay)));
	}

	void Application::PushLayer(std::unique_ptr<ILayer>&& layer)
	{
		/*	layer.get()->OnAttach();
			m_layers.emplace(std::make_pair(LAYER, std::move(layer)));*/
	}

	void Application::OnEvent(const Event& e)
	{
		//NLE_CORE_INFO("{0}", e.Print());

		Input::Instance().OnEvent(e);

		/*	for (auto& layer : m_layers)
				layer.second.get()->OnEvent(e);*/

	}


	Application::~Application()
	{
	}

}

