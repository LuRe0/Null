
//------------------------------------------------------------------------------
//
// File Name:	NStub.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "ScenePropertyPannel.h"
#include "Null/Engine/Submodules/Scene.h"
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>

//#include "backends/imgui_impl_opengl3.h"
//#include "backends/imgui_impl_glfw.h"

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	//void ScenePropertyPannel::SetContext(Scene* scene)
	//{
	//	m_Context = scene;
	//}

	void ScenePropertyPannel::OnImGUIRender()
	{
		ImGui::Begin("Scene Properties");
		Camera::CameraType type = m_Parent->GetCurrentCamera()->GetCameraType();
		CameraController* controller = m_Parent->GetCameraController();
		ImGui::Text("Camera:");
		ImGui::SameLine();
		ImGui::Text(magic_enum::enum_name(type).data());
		bool isToggled = type == Camera::ORTHOGRAPHIC;
		ImVec4 buttonColor = isToggled ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);

		// Create the toggle button with custom color
		if (ImGui::Button(isToggled ? "2D" : "3D"))
		{
			if (isToggled)
			{
				m_Parent->SetCamera(Camera::PERSPECTIVE);
			}
			else
			{
				m_Parent->SetCamera(Camera::ORTHOGRAPHIC);
			}
		}

		//ImGui::SameLine();
		//ImGui::Checkbox("Enable Camera", &controller->m_Enabled);
		ImGui::PushItemWidth(100);

		ImGui::DragFloat("Camera Speed", &controller->m_MovementSpeed, 0.5f, 0.0f, MAXSPEED);


		ImGui::SliderFloat("Mouse Sensitivity", &controller->m_MouseSensitivity, 0.0f, MAXSENSITIVITY);
		ImGui::SliderFloat("Scroll Sensitivity", &controller->m_ScrollSensitivity, 0.0f, MAXSENSITIVITY);
		ImGui::SliderFloat("Speed Scale", &controller->m_SpeedScale, 0.0f, MAXSCALE);

		ImGui::PopItemWidth();

		ImGui::PopStyleColor(3);


		ImGui::End();
	}
}
