
//------------------------------------------------------------------------------
//
// File Name:	NCameraManager.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NCameraManager.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    void NCameraManager::Load()
    {
    }

    void NCameraManager::Init()
    {
       NWindow* window = m_Parent->Get<NWindow>();
       m_CurrentCamera = AddCamera<Camera2D>("Default2D", window->Width(), window->Height());

      AddCamera<Camera3D>("Default3D", window->Width(), window->Height());


       for (auto& cam : m_Cameras3D)
           cam.second->Init();

       for (auto& cam : m_Cameras2D)
           cam.second->Init();
    }

    void NCameraManager::Update(float dt)
    {
        if (m_CurrentCamera) {
            m_CurrentCamera->Update(dt);
        }

        if (Input::KeyTriggered(GLFW_KEY_C))
        {
            if (dynamic_cast<Camera2D*>(m_CurrentCamera)) {
                m_CurrentCamera = GetCamera<Camera3D>("Default3D");
            }
            else if (dynamic_cast<Camera3D*>(m_CurrentCamera)) {
                m_CurrentCamera = GetCamera<Camera2D>("Default2D");
            }
            else {
                NLE_CORE_ERROR("Unknown camera type");
            }
        }
    }

    void NCameraManager::Unload()
    {
    }

    void NCameraManager::Shutdown()
    {
    }

    Camera* NCameraManager::GetCurrentCamera()
    {
        return m_CurrentCamera;
    }



    void NCameraManager::SetCurrentCamera(const std::string& name)
    {
        if (auto camera2D = GetCamera<Camera2D>(name)) {
            m_CurrentCamera = camera2D;
        }
        else if (auto camera3D = GetCamera<Camera3D>(name)) {
            m_CurrentCamera = camera3D;
        }
        else {
            m_CurrentCamera = nullptr;
        }
    }

}
