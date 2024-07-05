
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




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
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
		// Init each component
		for (int i = 0; i < m_Modules.size(); i++)
		{
			m_Modules[i].first->Update(dt);
		}
	}

	void NEngine::Shutdown()
	{
	}

	void NEngine::Add(IModule* component, const std::string_view& name)
	{
		component->SetParent(this);
		m_Modules.push_back({ component, std::string(name.data()) });
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