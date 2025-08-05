#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	CameraController.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//
const float SENSITIVITY = 0.1f;
const float SPEED = 250.5f;


const float MAXSPEED = 500;
const float MAXSENSITIVITY = 1.0f;
const float MAXSCALE = 10.0f;

//******************************************************************************//
// Private structures													        //
//******************************************************************************//


namespace NULLENGINE
{
    
	class Camera;

    class NLE_API CameraController 
    {
    public:

        virtual ~CameraController() = default;

        virtual void Init() = 0;

        virtual void Update(float dt)  = 0;

        virtual void Shutdown() = 0;

        virtual void SetCamera(Camera* camera) = 0;

        virtual void OnResize(unsigned int width, unsigned int height) = 0;

        virtual Camera* GetCamera() const = 0;

        void SetEnabled(bool enable) { m_Enabled = enable; if(!enable) m_FirstMouse = true; };
  
    protected:
        float m_MovementSpeed = 0.0f;

        float m_MouseSensitivity = 0.0f;

        float m_ScrollSensitivity = 0.0f;

        float m_SpeedScale = 2.0f;

        glm::vec2 m_LastMousePos = glm::vec2(0.0f,0.0f);

        bool m_FirstMouse = false;

        bool m_Enabled = true;

        friend class ScenePropertyPannel;
    };
}