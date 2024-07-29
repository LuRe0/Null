#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	OrthographicCameraController.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "CameraController.h"

//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//


namespace NULLENGINE
{
    class NLE_API Camera2D;

    class NLE_API OrthographicCameraController : public CameraController
    {
    public:

        OrthographicCameraController(float movementSpeed = SPEED, float mouseSensitivity = SENSITIVITY, float scrollSensitivity = SENSITIVITY*0.5f, Camera2D* camera = nullptr);
        ~OrthographicCameraController() override;
  

        void Init() override;

        // Add additional methods specific to orthographic camera control
        void Update(float dt) override;
   
        void Shutdown() override;

        void SetCamera(Camera* camera) override;

        Camera* GetCamera() const override;
    private:
        void ProcessKeyboardInput(float dt);
        void ProcessMouseInput(float dt);

        void OnMouseScrolled(const MouseScrolledEvent& e);
        void OnMouseMove(const MouseMoveEvent& e);

        Camera2D* m_Camera;
    };
}