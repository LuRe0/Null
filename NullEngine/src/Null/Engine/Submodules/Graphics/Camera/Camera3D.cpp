
//------------------------------------------------------------------------------
//
// File Name:	Camera.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Camera.h"
#include "Camera3D.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	Camera3D::Camera3D(int winWidth, int winHeight, glm::vec3 position, glm::vec3 up,
		float yaw, float pitch, float movementSpeed, float mouseSensitivity,
		float zoom, float nearclip, float farclip)
		: m_Position(position), m_Up(up), m_Yaw(yaw), m_Pitch(pitch),
		m_MovementSpeed(movementSpeed), m_MouseSensitivity(mouseSensitivity),
		m_Zoom(zoom), m_NearClip(nearclip), m_FarClip(farclip)
	{
		m_Position = position;
		m_WorldUp = up;
		m_Yaw = yaw;
		m_Pitch = pitch;
		m_Aspect = static_cast<float>(winWidth) / static_cast<float>(winHeight);
		m_CameraType = PERSPECTIVE;
		m_IsDirty = true;
	}

	void Camera3D::Init()
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();


		SUBSCRIBE_EVENT(WindowResizeEvent, &Camera3D::OnWindowResize, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(MouseScrolledEvent, &Camera3D::OnMouseScrolled, eventManager, EventPriority::Low);
	}

	void Camera3D::Update(float dt)
	{
		if (m_IsDirty)
		{
			UpdateCameraVectors();

			m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);

			UpdateProjectionMatrix();

			m_IsDirty = false;
		}

		float velocity = m_MovementSpeed * dt;

		if (Input::KeyDown(GLFW_KEY_W))
		{
			m_Position += m_Front * velocity;
			m_IsDirty = true;

		}
		if (Input::KeyDown(GLFW_KEY_S))
		{
			m_Position -= m_Front * velocity;
			m_IsDirty = true;

		}
		if (Input::KeyDown(GLFW_KEY_A))
		{
			m_Position -= m_Right * velocity;
			m_IsDirty = true;

		}
		if (Input::KeyDown(GLFW_KEY_D))
		{
			m_Position += m_Right * velocity;
			m_IsDirty = true;
		}
	}

	void Camera3D::SetZoom(float zoom)
	{
		m_Zoom = zoom;
	}

	const glm::mat4 Camera3D::GetViewMatrix() const
	{
		return m_ViewMatrix;
	}


	void Camera3D::OnWindowResize(const WindowResizeEvent& e)
	{
		//SetZoom(1280.0f / e.GetWidth());

		m_Aspect = e.GetHeight() != 0 ? e.GetWidth() / e.GetHeight() : 0;

		m_IsDirty = true;
	}

	void Camera3D::OnMouseScrolled(const MouseScrolledEvent& e)
	{
	    m_Zoom -= (float)e.GetYOffset();

		m_Zoom = std::clamp(m_Zoom, 1.0f, 45.0f);

		m_IsDirty = true;
	}

	void Camera3D::UpdateProjectionMatrix()
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(m_Zoom), m_Aspect, m_NearClip, m_FarClip);
	}

	void Camera3D::UpdateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}
}
