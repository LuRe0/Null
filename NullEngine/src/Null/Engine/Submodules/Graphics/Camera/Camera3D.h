#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Camera2D2D.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Camera.h"

//******************************************************************************//
// Definitions  														        //
//******************************************************************************//

/*
Code adapted from https://learnopengl.com
Made by Joey de Vries https://twitter.com/JoeyDeVriez\
LearnOpenGl license: https://creativecommons.org/licenses/by/4.0/legalcode
*/

//******************************************************************************//
// Private constants														    //
//******************************************************************************//
// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float ZOOM = 1.0f;
const float FOV = 45.0f;
//******************************************************************************//
// Private structures													        //
//******************************************************************************//


namespace NULLENGINE
{
    class Camera3D : public Camera {
    public:
        // constructor with vectors
        Camera3D(int winWidth, int winHeight, glm::vec3 position = glm::vec3(0.0f, 0.0f, 1000.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
                 float pitch = PITCH, float zoom = ZOOM, float fov = FOV, float nearclip = 0.1f, float farclip = 5000.0f);


        virtual void Init();

        virtual void Shudown() {};

        void Update(float dt) override;

        void SetPosition(const glm::vec3& position);
        void SetFront(const glm::vec3& front);
        void SetUp(const glm::vec3& up);
        void SetRight(const glm::vec3& right);
        void SetZoom(float zoom);
        void SetYaw(float yaw);
        void SetPitch(float pitch);
        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        const glm::mat4 GetViewMatrix() const override;


        const glm::vec3 GetPosition() const;
        const glm::vec3 GetUp() const;
        const glm::vec3 GetFront() const;
        const glm::vec3 GetRight() const;
        const float GetPitch() const;
        const float GetYaw() const;
        const float GetZoom() const;


        void View() override;

        void Write(JSON& json) override;


        void OnWindowResize(const WindowResizeEvent& e);
        void OnMouseScrolled(const MouseScrolledEvent& e);
    private:
        void UpdateProjectionMatrix();
        // calculates the front vector from the Camera's (updated) Euler Angles
        void UpdateCameraVectors();

        glm::vec3 m_Position;
        glm::vec3 m_Front;
        glm::vec3 m_Up;
        glm::vec3 m_Right;
        glm::vec3 m_WorldUp;

        // euler Angles
        float m_Yaw;
        float m_Pitch;

        float m_Zoom;

        float m_FOV;
        float m_Aspect;
        float m_NearClip = 0.1f;
        float m_FarClip =  5000.0f;
    };
}