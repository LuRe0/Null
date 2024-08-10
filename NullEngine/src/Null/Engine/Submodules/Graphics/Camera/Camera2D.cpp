
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
#include <misc/cpp/imgui_stdlib.h>




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
		m_CameraType = ORTHOGRAPHIC;
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
		//SUBSCRIBE_EVENT(MouseScrolledEvent, &Camera2D::OnMouseScrolled, eventManager, EventPriority::Low);
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
	}

	void Camera2D::View()
	{
		if (ImGui::InputText("Name", &m_Name))
		{
			if (m_Name.empty())
			{
				SetName("Default_Camera2D");
			}
		}

		if (m_Name == "Default_Camera2D")
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Assign unique name to camera");
		}

		if (ImGui::DragFloat2("Position", glm::value_ptr(m_Position), 0.5f) ||
			ImGui::SliderFloat("Rotation", &m_Rotation, -180.0f, 180.0f) ||
			ImGui::SliderFloat("Zoom", &m_Zoom, 0.25f, 5.0f))
			m_IsDirty = true;
	
	}

	void Camera2D::Write(JSON& json)
	{
		json["position"] = { m_Position.x, m_Position.y };
		json["zoom"] = m_Zoom;
		json["rotation"] = m_Rotation;
		json["type"] = "Camera2D";
		json["name"] = m_Name;
	}

	const glm::mat4 Camera2D::GetViewMatrix() const
	{
		return glm::inverse(m_ViewMatrix);
	}

	const glm::vec2 Camera2D::GetPosition() const
	{
		return m_Position;
	}

	float Camera2D::GetZoom() const
	{
		return m_Zoom;
	}

	void Camera2D::OnWindowResize(const WindowResizeEvent& e)
	{

		m_AspectRatio = e.GetHeight() != 0 ? static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight()) : m_AspectRatio;

		float deltaZoom = 1 - m_Zoom;

		float newZoom = e.GetHeight() != 0 ? (1280.0f / static_cast<float>(e.GetWidth()))+deltaZoom : m_Zoom;

		SetZoom(newZoom);

		float left = -static_cast<float>(e.GetWidth()) / 2.0f;
		float right = static_cast<float>(e.GetWidth()) / 2.0f;
		float bottom = -static_cast<float>(e.GetHeight()) / 2.0f;
		float top = static_cast<float>(e.GetHeight()) / 2.0f;

		SetProjection(left, right, bottom, top);

		m_IsDirty = true;
	}

	void Camera2D::OnMouseScrolled(const MouseScrolledEvent& e)
	{
		//m_Zoom -= e.GetYOffset() * 0.25f;

		//SetZoom(m_Zoom);
	}

}
