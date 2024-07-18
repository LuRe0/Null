
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

		AddCreateFunction<NWindow>([&engine]() { engine.Add<NULLENGINE::NWindow>(); });
		AddCreateFunction<NEventManager>([&engine]() { engine.Add<NULLENGINE::NEventManager>(); });
		AddCreateFunction<NCameraManager>([&engine]() { engine.Add<NULLENGINE::NCameraManager>(); });
		AddCreateFunction<NRegistry>([&engine]() { engine.Add<NULLENGINE::NRegistry>(); });
		AddCreateFunction<NShaderManager>([&engine]() { engine.Add<NULLENGINE::NShaderManager>(); });
		AddCreateFunction<NSceneManager>([&engine]() { engine.Add<NULLENGINE::NSceneManager>(); });
		AddCreateFunction<NRenderer>([&engine]() { engine.Add<NULLENGINE::NRenderer>(); });

		m_NullEngine = &engine;


		//vertex array
		//vertex buffer
		//index buffer
	}

	void Application::Load()
	{
		std::string filePath = std::string("Data/Modules/") + std::string("Modules") + std::string(".json");
		// Open the JSON file
		std::ifstream file(filePath);
		if (!file.is_open()) {
			NLE_ERROR("Error: Could not open file");
			return;
		}


		JSON j;
		file >> j;


		for (auto& sysJson : j["modules"]) {

			std::string type = sysJson["type"];

			m_Createfunctions.at(type)();
		}

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

