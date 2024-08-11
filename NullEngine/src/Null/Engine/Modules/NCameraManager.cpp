
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
        std::string filePath = std::string("../Assets/Cameras/") + std::string("Cameras") + std::string(".json");
        // Open the JSON file
        std::ifstream file(filePath);
        if (!file.is_open()) {
            NLE_ERROR("Error: Could not open file");
            return;
        }


        JSON j;
        file >> j;



        for (auto& cameraJson : j["cameras"]) {
     
            ReadCamera(cameraJson);
        }

        std::string defaultCam = j.value("DefaultCamera", "Default3D");

        m_CurrentCamera = GetCamera<Camera3D>(defaultCam);

        m_CurrentCamera = m_CurrentCamera == nullptr ? GetCamera<Camera2D>(defaultCam) : m_CurrentCamera;

        m_CurrentCamera = m_CurrentCamera == nullptr ? GetCamera<Camera3D>("Default3D") : m_CurrentCamera;

        if (!m_CurrentCamera)  m_CurrentCamera = GetCamera<Camera2D>("Default2D");

        NLE_CORE_ASSERT(m_CurrentCamera != nullptr, "No valid camera found!");

    }

    void NCameraManager::Init()
    {
        NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

        SUBSCRIBE_EVENT(EngineEditStateEvent, &NCameraManager::OnRuntimeStop, eventManager, EventPriority::High);


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

        //if (Input::KeyTriggered(GLFW_KEY_C))
        //{
        //    if (dynamic_cast<Camera2D*>(m_CurrentCamera)) {
        //        m_CurrentCamera = GetCamera<Camera3D>("Default3D");
        //    }
        //    else if (dynamic_cast<Camera3D*>(m_CurrentCamera)) {
        //        m_CurrentCamera = GetCamera<Camera2D>("Default2D");
        //    }
        //    else {
        //        NLE_CORE_ERROR("Unknown camera type");
        //    }
        //}
    }

    void NCameraManager::RuntimeUpdate(float dt)
    {
        Update(dt);
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


    Camera* NCameraManager::ReadCamera(const JSON& cameraJson)
    {
        NWindow* window = m_Parent->Get<NWindow>();

        std::string type = cameraJson["type"];
        std::string name = cameraJson["name"];
        NLE_CORE_ASSERT((type == "Camera2D" || type == "Camera3D"), "Camera type not reckognized: ", type);

        Camera* camera;
        if (type == "Camera2D")
        {
            camera = AddCamera<Camera2D>(name, window->Width(), window->Height(), cameraJson.value("zoom", 1.0f), cameraJson.value("rotation", 0.0f));
        }
        else
        {
            camera = AddCamera<Camera3D>(name, window->Width(), window->Height(), glm::vec3(cameraJson["position"][0], cameraJson["position"][1], cameraJson["position"][2]),
                glm::vec3(cameraJson["up"][0], cameraJson["up"][1], cameraJson["up"][2]), cameraJson.value("yaw", YAW), cameraJson.value("pitch", PITCH),
                cameraJson.value("zoom", ZOOM), cameraJson.value("fov", FOV), cameraJson.value("nearclip", 0.1f), cameraJson.value("farclip", 5000.0f));
        }

        camera->SetName(name);

        return camera;
    }

    void NCameraManager::OnRuntimeStop(const EngineEditStateEvent& e)
    {
        SetCurrentCamera("Editor3D");
    }



    void NCameraManager::SetCurrentCamera(const std::string& name)
    {
        if (auto camera2D = GetCamera<Camera2D>(name)) {
            m_CurrentCamera = camera2D;
        }
        else if (auto camera3D = GetCamera<Camera3D>(name)) {
            m_CurrentCamera = camera3D;
        }
        else 
        {
            //m_CurrentCamera = nullptr;

            NLE_CORE_WARN("Target Camera not found");
        }
    }

}
