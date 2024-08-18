#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	PerspectiveCameraController.h
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
    class NLE_API Camera3D;

    class NLE_API PerspectiveCameraController : public CameraController
    {
    public:

        PerspectiveCameraController(float movementSpeed = SPEED, float mouseSensitivity = SENSITIVITY, float scroll = SENSITIVITY*.5f, Camera3D* camera = nullptr);
        ~PerspectiveCameraController() override;
  

        void Init() override;

        // Add additional methods specific to orthographic camera control
        void Update(float dt) override;
   
        void Shutdown() override;

        void SetCamera(Camera* camera) override;

        Camera* GetCamera() const override;

        void OnResize(unsigned int width, unsigned int height) override;

    private:
        void ProcessKeyboardInput(float dt);
        void ProcessMouseInput(float dt);

        bool OnMouseScrolled(const MouseScrolledEvent& e);
        bool OnMouseMove(const MouseMoveEvent& e);

        bool m_ConstrainPitch = false;

        Camera3D* m_Camera;
    };
}