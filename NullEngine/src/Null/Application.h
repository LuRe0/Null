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
	class NLE_API ILayer;

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

		void PushOverlay(std::unique_ptr<ILayer>&& overlay);

		void PushLayer(std::unique_ptr<ILayer>&& layer);

		void OnEvent(const Event& event);

		virtual ~Application();
	private:
		enum LayerType
		{
			OVERLAY,
			LAYER,
		};

		//std::multimap<LayerType, std::vector<Layer>>

		std::multimap<LayerType, std::unique_ptr<ILayer>> m_layers;


		IEngine* m_NullEngine;


		Application(Application const&) = delete;
		Application& operator=(Application const&) = delete;
	};

	//To be defineded by user

	Application* CreateApplication();
}