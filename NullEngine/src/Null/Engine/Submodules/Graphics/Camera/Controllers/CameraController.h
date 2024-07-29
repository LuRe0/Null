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
    
    class NLE_API CameraController 
    {
    public:

        virtual ~CameraController() = default;

        virtual void Init() = 0;

        virtual void Update(float dt)  = 0;

        virtual void Shutdown() = 0;

        virtual void SetCamera(Camera* camera) = 0;


        virtual Camera* GetCamera() const = 0;

        void SetEnabled(bool enable) { m_Enabled = enable; if(!enable) m_FirstMouse = true; };
  
    protected:
        float m_MovementSpeed;

        float m_MouseSensitivity;

        float m_ScrollSensitivity;

        float m_SpeedScale = 2.0f;

        glm::vec2 m_LastMousePos;

        bool m_FirstMouse;

        bool m_Enabled = true;

        friend class ScenePropertyPannel;
    };
}