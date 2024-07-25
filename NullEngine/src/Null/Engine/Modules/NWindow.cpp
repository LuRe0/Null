#include "stdafx.h"
#include "NWindow.h"
#include "Null/Tools/Trace.h"
#include "Null/Engine/Submodules/Events/IEvents.h"
#include "imgui.h"
#include "GLFW/glfw3.h"
#include "backends/imgui_impl_glfw.h"

#include <glad/glad.h>

namespace NULLENGINE
{

	//#define SUBSCRIBE_EVENT(EVENT_TYPE, EVENT_CALLBACK, EVENT_MANAGER) \
	//    { \
	//        EventCallback<EVENT_TYPE> callback = std::bind(EVENT_CALLBACK, this, std::placeholders::_1); \
	//        std::unique_ptr<IEventHandler> handler = std::make_unique<EventHandler<EVENT_TYPE>>(callback, EVENT_TYPE::GetStaticEventType()); \
	//        EVENT_MANAGER->Subscribe(EVENT_TYPE::GetStaticEventType(), std::move(handler)); \
	//    }

	NWindow::NWindow(const std::string& name, uint32_t width, uint32_t height)
	{
		m_Data.m_Title = name;
		m_Data.m_Width = width;
		m_Data.m_Height = height;
		m_Data.m_Parent = this;
	}

	void NWindow::Load()
	{
		if (!InitGLFW()) {
			NLE_ERROR("Failed to initialize GLFW");
		}
		else {
			m_Window = InitializeWindow(1280, 720, m_Data.m_Title.c_str());
			if (!m_Window) {
				NLE_ERROR("Failed to create GLFW window");
			}
			else {
				//SetFramebufferSizeCallback(window, framebuffer_size_callback);
				NLE_TRACE("Creating Window {0} ({1}, {2})", m_Data.m_Title.c_str(), 1280, 720);
			}
		}
	}

	void NWindow::Init()
	{
		NEventManager* eventManager = m_Parent->Get<NEventManager>();

		InitializeWindowEvents(eventManager);



		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, WindowResizeCallback);
		glfwSetWindowCloseCallback(m_Window, WindowCloseCallback);
		//glfwSetKeyCallback(m_Window, KeyCallback);
		//glfwSetCharCallback(m_Window, KeyTypeCallback);
		//glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);
		//glfwSetCursorPosCallback(m_Window, MouseMoveCallback);
		//glfwSetScrollCallback(m_Window, MouseScrollCallback);


		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			if (ImGui::GetCurrentContext())
				ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
			KeyCallback(window, key, scancode, action, mods);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
			if (ImGui::GetCurrentContext())
				ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
			MouseButtonCallback(window, button, action, mods);
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
			if (ImGui::GetCurrentContext())
				ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
			MouseScrollCallback(window, xoffset, yoffset);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
			if (ImGui::GetCurrentContext())
				ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
			MouseMoveCallback(window, xpos, ypos);
			});



		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int c) {
			if (ImGui::GetCurrentContext())
				ImGui_ImplGlfw_CharCallback(window, c);
			KeyTypeCallback(window, c);
			});

	}

	void NWindow::Update(float dt)
	{

		if (m_Window)
		{
			// Update loop: render, poll events, etc.

			glfwPollEvents();


		}
	}

	void NWindow::Unload()
	{

	}

	void NWindow::Shutdown()
	{
		TerminateGLFW();
	}

	bool NWindow::WindowClosed()
	{
		return !m_Window || glfwWindowShouldClose(m_Window);
	}

	bool NWindow::WindowMinimized()
	{
		return (Width() && Height() == 0);
	}

	void NWindow::CloseWindow()
	{
		glfwSetWindowShouldClose(m_Window, true);
	}

	void NWindow::InitializeWindowEvents(NEventManager* eventManager)
	{
		//EventCallback<WindowResizeEvent> callback = std::bind(&NWindow::OnWindowResize, this, std::placeholders::_1);;

		//std::unique_ptr<IEventHandler> handler = std::make_unique<EventHandler<WindowResizeEvent>>(callback, Event::WindowResize);
		//eventManager->Subscribe(WindowResizeEvent::GetStaticEventType(), std::move(handler));



	}

	bool NWindow::InitGLFW()
	{
		if (!glfwInit()) {
			NLE_ERROR("Failed to create GLFW window");
			return false;
		}

		return true;
	}

	GLFWwindow* NWindow::InitializeWindow(int width, int height, const char* title)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
		if (!window) {
			NLE_ERROR("Failed to create GLFW window");
			glfwTerminate();
			return nullptr;
		}


		m_Data.m_Height = height;
		m_Data.m_Width = width;
		m_Data.m_Parent = this;

		glfwMakeContextCurrent(window);
		glfwSetWindowUserPointer(window, &m_Data);
		SetVSynch(true);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		status ? NLE_CORE_TRACE("GLAD initialized successfully.") : NLE_CORE_ERROR("Failed to initialize GLAD!");

		return window;
	}

	void NWindow::TerminateGLFW()
	{
	}

	void NWindow::SetVSynch(bool s)
	{
		glfwSwapInterval(s);

		m_Data.m_VSynch = s;
	}

	void NWindow::SetBlockEvents(bool b)
	{
		m_Data.m_BlockEvents = b;
	}

	//void NWindow::SetFramebufferSizeCallback(GLFWwindow* window, GLFWframebuffersizefun callback)
	//{
	//    glfwSetFramebufferSizeCallback(window, callback);
	//}


	///GLFW Callbacks
	void  NWindow::WindowResizeCallback(GLFWwindow* window, int width, int height)
	{
		Window& data = *((Window*)glfwGetWindowUserPointer(window));

		data.m_Width = width;
		data.m_Height = height;

		data.m_callbackFunc(WindowResizeEvent(width, height));

	}

	void  NWindow::WindowCloseCallback(GLFWwindow* window)
	{
		Window& data = *((Window*)glfwGetWindowUserPointer(window));

		data.m_callbackFunc(WindowCloseEvent());

	}

	void  NWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Window& data = *((Window*)glfwGetWindowUserPointer(window));

		if (!data.m_BlockEvents)
		{


			switch (action)
			{
			case GLFW_PRESS:
			{


				data.m_callbackFunc(KeyPressEvent(key, 0));
				break;
			}
			case GLFW_RELEASE:
			{


				data.m_callbackFunc(KeyReleaseEvent(key));

				break;
			}
			case GLFW_REPEAT:
			{
				data.m_callbackFunc(KeyHoldEvent(key));

				break;
			}
			default:
				break;
			}
		}
	}

	void NWindow::KeyTypeCallback(GLFWwindow* window, unsigned int keycode)
	{

		Window& data = *((Window*)glfwGetWindowUserPointer(window));
		if (!data.m_BlockEvents)
		{

			data.m_callbackFunc(KeyTypedEvent(keycode));
		}
	}

	void  NWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		Window& data = *((Window*)glfwGetWindowUserPointer(window));

		if (!data.m_BlockEvents)
		{
			switch (action)
			{
			case GLFW_PRESS:
			{


				data.m_callbackFunc(MouseButtonPressEvent(button));

				break;
			}
			case GLFW_RELEASE:
			{


				data.m_callbackFunc(MouseButtonReleaseEvent(button));

				break;
			}
			default:
				break;
			}

			//Input::Instance().SetMouseState(button, action);
		}
	}

	void  NWindow::MouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
	{
		Window& data = *((Window*)glfwGetWindowUserPointer(window));

		if (!data.m_BlockEvents)
		{

			data.m_callbackFunc(MouseMoveEvent(static_cast<float>(xpos), static_cast<float>(ypos)));

			//Input::Instance().SetMousePos(xpos, ypos);
		}
	}

	void  NWindow::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		Window& data = *((Window*)glfwGetWindowUserPointer(window));

		if (!data.m_BlockEvents)
		{

			data.m_callbackFunc(MouseScrolledEvent(static_cast<float>(xoffset), static_cast<float>(yoffset)));
		}
	}


}
