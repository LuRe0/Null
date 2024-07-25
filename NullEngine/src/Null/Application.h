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
#include "Null/Engine/Submodules/Layers/Layer.h"
#include "Null//Engine/Submodules/Graphics/Shader/Shader.h"

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
	class NLE_API Application;
	class NLE_API ImGuiLayer;
	//class NLE_API Shader;

	class NLE_API Application
	{
	public:
		Application(const std::string& name = "Null Engine");

		virtual void Load();
		//! Virtual Init function
		virtual void Init();
		//! Virtual Update function
		virtual void Update();

		virtual void Unload();
		//! Virtual Shutdown function
		virtual void Shutdown();

		void PushOverlay(std::unique_ptr<ILayer>&& overlay);

		void PushLayer(std::unique_ptr<ILayer>&& layer);

		void OnEvent(const Event& event);

		template <typename T>
		void AddCreateFunction(std::function<void() > function)
		{
			m_Createfunctions.emplace(System<T>::TypeName(), function);
		}

		const std::string Name() const { return m_ApplicationName; }

		virtual ~Application();
	private:
		enum LayerType
		{
			OVERLAY,
			LAYER,
		};

		//std::multimap<LayerType, std::vector<Layer>>

		//std::multimap<LayerType, std::unique_ptr<ILayer>> m_layers;

		std::vector<std::unique_ptr<ILayer>> m_layers;

		//std::unique_ptr<ImGuiLayer> m_ImGuiLayer;

		IEngine* m_NullEngine;
		std::unordered_map<std::string, std::function<void()>> m_Createfunctions;

		std::string m_ApplicationName;

		Application(Application const&) = delete;
		Application& operator=(Application const&) = delete;
	};

	//To be defineded by user

	Application* CreateApplication();
}