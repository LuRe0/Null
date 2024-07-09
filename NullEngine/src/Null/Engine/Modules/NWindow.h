#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NWindow.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include "Null/Engine/Submodules/Events/CommonEvents.h"
#include <GLFW/glfw3.h>

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
	class NLE_API NEventManager;
	//struct GLFWwindow;

	class NLE_API NWindow : public IModule
	{
	public:

		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

		bool WindowClosed();

		void InitializeWindowEvents(NEventManager* eventManager);

		// GLFW-specific functions
		static bool InitGLFW();
		GLFWwindow* InitializeWindow(int width, int height, const char* title);
		static void TerminateGLFW();
		//static void SetFramebufferSizeCallback(GLFWwindow* window, GLFWframebuffersizefun callback);

		static const std::string Name() { return "Window"; }

		const unsigned int Height() { return m_Data.m_Height;  }

		const unsigned int Width() { return m_Data.m_Width;  }

		void SetVSynch(bool s) { m_Data.m_VSynch = s; }

		bool IsVSynch(bool s) { return m_Data.m_VSynch; }

		void ResizeWindowCallback(const WindowResizeEvent& event);

	private:
		// Private data members
		GLFWwindow* m_Window = nullptr;

		struct Window
		{
			std::string m_Title;

			//! Desired window width
			unsigned int m_Width;

			//! Desired window width
			unsigned int m_Height;

			bool m_VSynch;

			//EventCallback<Event> m_callbackFunc;

			NWindow* m_Parent;
		};

		Window m_Data;


		// GLFW Callback function declarations
		static void WindowResizeCallback(GLFWwindow* window, int width, int height);
		static void WindowCloseCallback(GLFWwindow* window);
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
		static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);


		// Member functions for handling events
		void OnWindowResize(const WindowResizeEvent& event);
		void OnWindowClose(const WindowCloseEvent& event);
		void OnKey(const KeyEvent& event);
		void OnMouseButton(const MouseButtonEvent& event);
		void OnMouseMove(const MouseMoveEvent& event);
		void OnMouseScroll(const MouseScrolledEvent& event);


	};




}
