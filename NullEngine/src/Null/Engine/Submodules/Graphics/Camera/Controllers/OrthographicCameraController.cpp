
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
#include "OrthographicCameraController.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	OrthographicCameraController::OrthographicCameraController(float movementSpeed, float mouseSensitivity, float scrollSensitivity, Camera2D* camera)
	{
		m_MovementSpeed = movementSpeed;
		m_MouseSensitivity = mouseSensitivity;
		m_ScrollSensitivity = scrollSensitivity;
		m_Camera = camera;
	}

	OrthographicCameraController::~OrthographicCameraController()
	{
	}

	void OrthographicCameraController::Init()
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		SUBSCRIBE_EVENT(MouseScrolledEvent, &OrthographicCameraController::OnMouseScrolled, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(MouseMoveEvent, &OrthographicCameraController::OnMouseMove, eventManager, EventPriority::Low);
	}

	void OrthographicCameraController::Update(float dt)
	{
		if (!m_Enabled)
			return;

		ProcessKeyboardInput(dt);
		ProcessMouseInput(dt);

	}

	void OrthographicCameraController::Shutdown()
	{
	}

	void OrthographicCameraController::SetCamera(Camera* camera)
	{
		m_Camera = dynamic_cast<Camera2D*>(camera);;
	}


	Camera* OrthographicCameraController::GetCamera() const
	{
		return m_Camera;
	}

	void OrthographicCameraController::OnResize(unsigned int width, unsigned int height)
	{
		m_Camera->OnWindowResize(WindowResizeEvent(width, height));
	}

	void OrthographicCameraController::ProcessKeyboardInput(float dt)
	{
		float velocity = m_MovementSpeed * m_SpeedScale * dt;

		glm::vec2 position = m_Camera->GetPosition();

		if (Input::KeyDown(GLFW_KEY_W))
		{

			position.y += velocity;
			m_Camera->SetPosition(position);
		}
		if (Input::KeyDown(GLFW_KEY_S))
		{
			position.y -= velocity;
			m_Camera->SetPosition(position);

		}
		if (Input::KeyDown(GLFW_KEY_A))
		{
			position.x -= velocity;
			m_Camera->SetPosition(position);


		}
		if (Input::KeyDown(GLFW_KEY_D))
		{
			position.x += velocity;
			m_Camera->SetPosition(position);

		}
	}

	void OrthographicCameraController::ProcessMouseInput(float dt)
	{
		//if ()
		//{
		//	m_MouseClickPos = Input::GetMouseXY();
		//	NLE_TRACE("Click Pos: {0}, {1}", m_MouseClickPos.x, m_MouseClickPos.y);
		//}
	}

	bool OrthographicCameraController::OnMouseScrolled(const MouseScrolledEvent& e)
	{
		if (!m_Enabled)
			return false;

		float zoom = m_Camera->GetZoom();

		zoom -= e.GetYOffset() * m_MouseSensitivity;

		m_Camera->SetZoom(zoom);
		return true;
	}

	bool OrthographicCameraController::OnMouseMove(const MouseMoveEvent& e)
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

			glm::vec2 position = m_Camera->GetPosition();

			m_Camera->SetPosition(position + deltaMouse);
		}
		else
		{
			// Reset mouse flag when control is not held
			m_FirstMouse = true;
		}
		return true;
	}

}
