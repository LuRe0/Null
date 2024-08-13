
//------------------------------------------------------------------------------
//
// File Name:	NDebugManager.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NDebugManager.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    void NDebugManager::Load()
    {
    }

    void NDebugManager::Init()
    {
        NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

    }

    void NDebugManager::Update(float dt)
    {
    }

    void NDebugManager::RuntimeUpdate(float dt)
    {
        Update(dt);
    }

    void NDebugManager::Unload()
    {
    }

    void NDebugManager::Shutdown()
    {
    }

}
