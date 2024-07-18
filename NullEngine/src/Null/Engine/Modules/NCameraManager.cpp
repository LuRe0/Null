
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
        std::string filePath = std::string("Data/Cameras/") + std::string("Cameras") + std::string(".json");
        // Open the JSON file
        std::ifstream file(filePath);
        if (!file.is_open()) {
            NLE_ERROR("Error: Could not open file");
            return;
        }


        JSON j;
        file >> j;


        NWindow* window = m_Parent->Get<NWindow>();

        for (auto& cameraJson : j["cameras"]) {
     
            std::string type = cameraJson["type"];
            NLE_CORE_ASSERT((type == "Camera2D" || type == "Camera3D"), "Camera type not reckognized: ", type);


            if (type == "Camera2D")
            {
                AddCamera<Camera2D>("Default2D", window->Width(), window->Height(), cameraJson.value("zoom", 1.0f), cameraJson.value("rotation", 0.0f));
            }
            else
            {
                AddCamera<Camera3D>("Default3D", window->Width(), window->Height(), glm::vec3(cameraJson["position"][0], cameraJson["position"][1], cameraJson["position"][2]), 
                                     glm::vec3(cameraJson["up"][0], cameraJson["up"][1], cameraJson["up"][2]), cameraJson.value("yaw", YAW), cameraJson.value("pitch", PITCH),
                                     cameraJson.value("movementSpeed", SPEED), cameraJson.value("mouseSensitivity", SENSITIVITY), cameraJson.value("zoom", ZOOM),
                                     cameraJson.value("nearclip", 0.1f), cameraJson.value("farclip", 5000.0f));
            }

             
        }

        std::string defaultCam = j.value("DefaultCamera", "Default3D");

        m_CurrentCamera = GetCamera<Camera3D>(defaultCam);

        m_CurrentCamera = m_CurrentCamera == nullptr ? GetCamera<Camera3D>("Default3D") : m_CurrentCamera;

        if (!m_CurrentCamera)  m_CurrentCamera = GetCamera<Camera2D>("Default2D");

        NLE_CORE_ASSERT(m_CurrentCamera != nullptr, "No valid camera found!");

    }

    void NCameraManager::Init()
    {

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
