
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
#include "Camera2D.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	Camera2D::Camera2D(int windowWidth, int windowHeight, float zoom, float rotation) : m_Position(0.0f), m_Rotation(0.0f), m_Zoom(zoom), m_MovementSpeed(250)
	{
		m_AspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

		float left = -static_cast<float>(windowWidth) / 2.0f;
		float right = static_cast<float>(windowWidth) / 2.0f;
		float bottom = -static_cast<float>(windowHeight) / 2.0f;
		float top = static_cast<float>(windowHeight) / 2.0f;

		m_ProjectionMatrix = glm::ortho(left, right, bottom, top);

		m_IsDirty = true;
	}
	void Camera2D::SetPosition(const glm::vec2& position)
	{
		m_Position = position;
		m_IsDirty = true;
	}
	void Camera2D::SetRotation(float rotation)
	{
		m_Rotation = rotation;
		m_IsDirty = true;
	}
	void Camera2D::SetZoom(float zoom)
	{
		m_Zoom = zoom;

		m_Zoom = std::clamp(m_Zoom, 0.25f, 5.0f);

		m_IsDirty = true;

	}
	void Camera2D::SetProjection(float left, float right, float bottom, float top)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top);
	}
	void Camera2D::Init()
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();


		SUBSCRIBE_EVENT(WindowResizeEvent, &Camera2D::OnWindowResize, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(MouseScrolledEvent, &Camera2D::OnMouseScrolled, eventManager, EventPriority::Low);
	}
	void Camera2D::Update(float dt)
	{
		if (m_IsDirty)
		{
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(m_Position, 0.0f));
			glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(m_Zoom * m_AspectRatio, m_Zoom * m_AspectRatio, 1.0f));

			m_ViewMatrix = translationMatrix * rotationMatrix * scaleMatrix;


			m_IsDirty = false;
		}

		float velocity = m_MovementSpeed * m_Zoom * dt;

		if (Input::KeyDown(GLFW_KEY_W))
		{
			m_Position.y += velocity;
			m_IsDirty = true;

		}
		if (Input::KeyDown(GLFW_KEY_S))
		{
			m_Position.y -= velocity;
			m_IsDirty = true;

		}
		if (Input::KeyDown(GLFW_KEY_A))
		{
			m_Position.x -= velocity;
			m_IsDirty = true;

		}
		if (Input::KeyDown(GLFW_KEY_D))
		{
			m_Position.x += velocity;
			m_IsDirty = true;
		}

	}

	const glm::mat4 Camera2D::GetViewMatrix() const
	{
		return glm::inverse(m_ViewMatrix);
	}

	void Camera2D::OnWindowResize(const WindowResizeEvent& e)
	{

		//SetZoom(1280.0f / e.GetWidth());

		m_AspectRatio = e.GetHeight() != 0 ? e.GetWidth() / e.GetHeight() : 0;


		float left = -static_cast<float>(e.GetWidth()) / 2.0f;
		float right = static_cast<float>(e.GetWidth()) / 2.0f;
		float bottom = -static_cast<float>(e.GetHeight()) / 2.0f;
		float top = static_cast<float>(e.GetHeight()) / 2.0f;

		SetProjection(left, right, bottom, top);

		m_IsDirty = true;
	}

	void Camera2D::OnMouseScrolled(const MouseScrolledEvent& e)
	{
		m_Zoom -= e.GetYOffset() * 0.25f;

		SetZoom(m_Zoom);
	}

}
