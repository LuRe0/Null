
//------------------------------------------------------------------------------
//
// File Name:	NEngine.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NEngine.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include "Null/Tools/Trace.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void NEngine::Load()
	{
		// Init each component
		for (int i = 0; i < m_Modules.size(); i++)
		{
			m_Modules[i].first->Load();
		}
	}
	void NEngine::Init()
	{
		// Init each component
		for (int i = 0; i < m_Modules.size(); i++)
		{
			m_Modules[i].first->Init();
		}

	}

	void NEngine::Update(float dt)
	{
		// Update and draw each component
		for (int i = 0; i < m_Modules.size(); i++)
		{
			if (m_EditorEnabled)
			{
				switch (m_EngineState)
				{
				case NULLENGINE::IEngine::EDIT:
					m_Modules[i].first->Update(dt);
					break;
				case NULLENGINE::IEngine::PAUSE:
					break;
				case NULLENGINE::IEngine::RUN_MAXIMIZED: // Handle maximized state
				case NULLENGINE::IEngine::RUN_WINDOWED:  // Handle windowed state
					m_Modules[i].first->RuntimeUpdate(dt);
					break;
				case NULLENGINE::IEngine::SIMULATE:
					break;
				default:
					break;
				}
			}
			else
				m_Modules[i].first->RuntimeUpdate(dt);

			m_Modules[i].first->Render();
		}
	}

	void NEngine::Unload()
	{
		// Unload each component
		for (int i = 0; i < m_Modules.size(); i++)
		{
			m_Modules[i].first->Unload();
		}
	}

	void NEngine::Shutdown()
	{
		// Shutdown each component
		for (int i = 0; i < m_Modules.size(); i++)
		{
			m_Modules[i].first->Shutdown();
		}
	}

	void NEngine::Add(IModule* component, const std::string_view& name)
	{
		if (component)
		{
			component->SetParent(this);
			m_Modules.push_back({ component, std::string(name.data()) });
			std::string message = "Added " + (std::string)name + " To Engine";
			NLE_TRACE(message);
			return;
		}

		std::string message = "Failed to add " + (std::string)name + " To Engine";
		NLE_TRACE(message);
	}

	IModule* NEngine::Get(const std::string_view& name) const
	{
		for (int i = 0; i < m_Modules.size(); ++i)
		{
			if (m_Modules[i].second == name)
			{
				return m_Modules[i].first;
			}
		}
		return nullptr;
	}

}