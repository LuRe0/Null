
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
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		NWindow* window = NEngine::Instance().Get<NWindow>();
		ImGui_ImplGlfw_InitForOpenGL(window->GetWinddow(), true);
		ImGui_ImplOpenGL3_Init("#version 410");

		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		//SUBSCRIBE_EVENT(WindowResizeEvent, &ImGuiLayer::OnWindowResize, eventManager);
		//SUBSCRIBE_EVENT(WindowCloseEvent, &ImGuiLayer::OnWindowClose, eventManager);
		//SUBSCRIBE_EVENT(KeyPressEvent, &ImGuiLayer::OnKeyPressed, eventManager);
		//SUBSCRIBE_EVENT(KeyTypedEvent, &ImGuiLayer::OnKeyTyped, eventManager);
		//SUBSCRIBE_EVENT(KeyReleaseEvent, &ImGuiLayer::OnKeyReleased, eventManager);
		//SUBSCRIBE_EVENT(MouseButtonPressEvent, &ImGuiLayer::OnMousePressed, eventManager);
		//SUBSCRIBE_EVENT(MouseButtonReleaseEvent, &ImGuiLayer::OnMouseReleased, eventManager);
		//SUBSCRIBE_EVENT(MouseMoveEvent, &ImGuiLayer::OnMouseMove, eventManager);
		//SUBSCRIBE_EVENT(MouseScrolledEvent, &ImGuiLayer::OnMouseScroll, eventManager);

	}
	void ImGuiLayer::OnUpdate(float dt)
	{
		//NWindow* window = NEngine::Instance().Get<NWindow>();

		//ImGuiIO& io = ImGui::GetIO();
		//io.DisplaySize = ImVec2(window->Width(), window->Height());

		//ImGui_ImplOpenGL3_NewFrame();
		//ImGui::NewFrame();

		//static bool show = true;

		//ImGui::ShowDemoWindow(&show);

		//ImGui::Render();
		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		//{
		//	GLFWwindow* backup_current_context = window->GetWinddow();
		//	ImGui::UpdatePlatformWindows();
		//	ImGui::RenderPlatformWindowsDefault();
		//	glfwMakeContextCurrent(backup_current_context);
		//}

	}
	void ImGuiLayer::OnRender()
	{
		static bool show = true;

		ImGui::ShowDemoWindow(&show);
	}
	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(const Event& e) 
	{
		//NLE_INFO("{0}", e.Print());

		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		eventManager->TriggerEvent(e);
	}

	void ImGuiLayer::Begin()
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		NWindow* window = NEngine::Instance().Get<NWindow>();

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(window->Width(), window->Height());

		// Rendering
		ImGui::Render();

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = window->GetWinddow();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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