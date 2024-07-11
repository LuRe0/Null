
//------------------------------------------------------------------------------
//
// File Name:	ImGuiLayer.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "ImGuiLayer.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

// temporary

#include <GLFW/glfw3.h>
#include <glad/glad.h>


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		NWindow* window = NEngine::Instance().Get<NWindow>();
		ImGui_ImplGlfw_InitForOpenGL(window->GetWinddow(), true);
		ImGui_ImplOpenGL3_Init("#version 410");

		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		SUBSCRIBE_EVENT(WindowResizeEvent, &ImGuiLayer::OnWindowResize, eventManager);
		SUBSCRIBE_EVENT(WindowCloseEvent, &ImGuiLayer::OnWindowClose, eventManager);
		SUBSCRIBE_EVENT(KeyPressEvent, &ImGuiLayer::OnKeyPressed, eventManager);
		SUBSCRIBE_EVENT(KeyTypedEvent, &ImGuiLayer::OnKeyTyped, eventManager);
		SUBSCRIBE_EVENT(KeyReleaseEvent, &ImGuiLayer::OnKeyReleased, eventManager);
		SUBSCRIBE_EVENT(MouseButtonPressEvent, &ImGuiLayer::OnMousePressed, eventManager);
		SUBSCRIBE_EVENT(MouseButtonReleaseEvent, &ImGuiLayer::OnMouseReleased, eventManager);
		SUBSCRIBE_EVENT(MouseMoveEvent, &ImGuiLayer::OnMouseMove, eventManager);
		SUBSCRIBE_EVENT(MouseScrolledEvent, &ImGuiLayer::OnMouseScroll, eventManager);

	}
	void ImGuiLayer::OnUpdate(float dt)
	{
		NWindow* window = NEngine::Instance().Get<NWindow>();

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(window->Width(), window->Height());

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		static bool show = true;

		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	}
	void ImGuiLayer::OnDetach()
	{
	}

	void ImGuiLayer::OnEvent(const Event& e) 
	{
		NLE_INFO("{0}", e.Print());

		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		eventManager->TriggerEvent(e);
	}



	void ImGuiLayer::OnWindowResize(const WindowResizeEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
		io.DisplayFramebufferScale =  ImVec2(1.0f, 1.0f);
		glViewport(0, 0, e.GetWidth(), e.GetHeight());
	}

	void ImGuiLayer::OnWindowClose(const WindowCloseEvent& e)
	{
		NLE_CORE_INFO("{0}", e.Print());
	}

	void ImGuiLayer::OnKeyPressed(const KeyPressEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.KeysDown[e.GetKeyCode()] = true;

		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

	}

	void ImGuiLayer::OnKeyReleased(const KeyReleaseEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.KeysDown[e.GetKeyCode()] = false;
	}

	void ImGuiLayer::OnKeyTyped(const KeyTypedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		int keycode = e.GetKeyCode();

		if (keycode > 0 && keycode < 0x10000)
			io.AddInputCharacter((unsigned short)keycode);
	}

	//void ImGuiLayer::OnKeyHold(const KeyHoldEvent& event)
	//{
	//}

	void ImGuiLayer::OnMousePressed(const MouseButtonPressEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.MouseDown[e.GetButton()] = true;
	}


	void ImGuiLayer::OnMouseReleased(const MouseButtonReleaseEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.MouseDown[e.GetButton()] = false;
	}

	void ImGuiLayer::OnMouseMove(const MouseMoveEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.MousePos = ImVec2(e.GetX(), e.GetY());
	}

	void ImGuiLayer::OnMouseScroll(const MouseScrolledEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.MouseWheelH += e.GetXOffset();
		io.MouseWheel += e.GetYOffset();

	}
}