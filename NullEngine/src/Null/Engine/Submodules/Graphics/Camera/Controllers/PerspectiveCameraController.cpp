
//------------------------------------------------------------------------------
//
// File Name:	CameraController.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "CameraController.h"
#include "PerspectiveCameraController.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	PerspectiveCameraController::PerspectiveCameraController(float movementSpeed, float mouseSensitivity, float scrollSensitivity, Camera3D* camera)
	{
		m_MovementSpeed = movementSpeed;
		m_MouseSensitivity = mouseSensitivity;
		m_ScrollSensitivity = mouseSensitivity;
		m_Camera = camera;
	}

	PerspectiveCameraController::~PerspectiveCameraController()
	{
	}

	void PerspectiveCameraController::Init()
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		SUBSCRIBE_EVENT(MouseScrolledEvent, &PerspectiveCameraController::OnMouseScrolled, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(MouseMoveEvent, &PerspectiveCameraController::OnMouseMove, eventManager, EventPriority::Low);
	}

	void PerspectiveCameraController::Update(float dt)
	{
		if (!m_Enabled)
			return;

		ProcessKeyboardInput(dt);
		ProcessMouseInput(dt);

	}

	void PerspectiveCameraController::Shutdown()
	{
	}

	void PerspectiveCameraController::SetCamera(Camera* camera)
	{
		m_Camera = dynamic_cast<Camera3D*>(camera);;
	}

	Camera* PerspectiveCameraController::GetCamera() const
	{
		return m_Camera;
	}

	void PerspectiveCameraController::OnResize(unsigned int width, unsigned int height)
	{
		m_Camera->OnWindowResize(WindowResizeEvent(width, height));
	}

	void PerspectiveCameraController::ProcessKeyboardInput(float dt)
	{
		float velocity = m_MovementSpeed * m_SpeedScale * dt;
		glm::vec3 front = m_Camera->GetFront();
		glm::vec3 position = m_Camera->GetPosition();
		glm::vec3 right = m_Camera->GetRight();
		glm::vec3 up = m_Camera->GetUp();
		if (Input::KeyDown(GLFW_KEY_W))
		{
			position += front * velocity;
			m_Camera->SetPosition(position);
		}
		if (Input::KeyDown(GLFW_KEY_S))
		{
			position -= front * velocity;
			m_Camera->SetPosition(position);

		}
		if (Input::KeyDown(GLFW_KEY_A))
		{
			position -= right * velocity;
			m_Camera->SetPosition(position);

		}
		if (Input::KeyDown(GLFW_KEY_D))
		{
			position += right * velocity;
			m_Camera->SetPosition(position);
		}

		if (Input::KeyDown(GLFW_KEY_Q))
		{
			position += up * velocity;
			m_Camera->SetPosition(position);
		}

		if (Input::KeyDown(GLFW_KEY_E))
		{
			position -= up * velocity;
			m_Camera->SetPosition(position);
		}
	}

	void PerspectiveCameraController::ProcessMouseInput(float dt)
	{
		//if ()
		//{
		//	m_MouseClickPos = Input::GetMouseXY();
		//	NLE_TRACE("Click Pos: {0}, {1}", m_MouseClickPos.x, m_MouseClickPos.y);
		//}
	}

	bool PerspectiveCameraController::OnMouseScrolled(const MouseScrolledEvent& e)
	{
		if (!m_Enabled)
			return false;

		float zoom = m_Camera->GetZoom();

		zoom -= e.GetYOffset() * m_MouseSensitivity;

		m_Camera->SetZoom(zoom);

		return true;
	}

	bool PerspectiveCameraController::OnMouseMove(const MouseMoveEvent& e)
	{
		if (!m_Enabled)
			return false;

		float xpos = static_cast<float>(e.GetX());
		float ypos = static_cast<float>(e.GetY());

		if (Input::MouseHold(GLFW_MOUSE_BUTTON_2))
		{
			if (m_FirstMouse)
			{
				m_LastMousePos = glm::vec2(xpos, ypos);
				m_FirstMouse = false;
			}

			glm::vec2 deltaMouse = glm::vec2(xpos - m_LastMousePos.x, m_LastMousePos.y - ypos);
 
			m_LastMousePos = glm::vec2(xpos, ypos);

			deltaMouse *= m_MouseSensitivity * m_Camera->GetZoom();

			float yaw = m_Camera->GetYaw();
			float pitch = m_Camera->GetPitch();


			if (m_ConstrainPitch)
			{
				if (pitch > 89.0f)
					pitch = 89.0f;
				if (pitch < -89.0f)
					pitch = -89.0f;
			}

			m_Camera->SetYaw(yaw + deltaMouse.x);
			m_Camera->SetPitch(pitch + deltaMouse.y);


		}
		else
		{
			// Reset mouse flag when control is not held
			m_FirstMouse = true;
		}


		return true;
	}

}
