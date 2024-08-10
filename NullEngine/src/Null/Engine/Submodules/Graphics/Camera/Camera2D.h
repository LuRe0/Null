#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Camera.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Camera.h"
#include <WinUser.h>



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
    class Camera2D : public Camera {
    public:
        Camera2D(int windowWidth, int windowHeight, float zoom = 1.0f, float rotation = 0);

        void SetPosition(const glm::vec2& position);

        void SetRotation(float rotation);

        void SetZoom(float zoom);

        virtual void Init();

        virtual void Shudown() {};

        void Update(float dt) override;
        void View() override;

        void Write(JSON& json) override;

        const glm::mat4 GetViewMatrix() const override;

        const glm::vec2 GetPosition() const;
        float GetZoom() const;


        void OnWindowResize(const WindowResizeEvent& e);
        void OnMouseScrolled(const MouseScrolledEvent& e);

        //static void OnMouseMoved(const MouseMoveEvent& event);

    private:
        glm::vec2 m_Position;

        float m_MovementSpeed;
        float m_Zoom;
        float m_Rotation;
        float m_AspectRatio;

        void SetProjection(float left,float right,float bottom,float top);
    };


}