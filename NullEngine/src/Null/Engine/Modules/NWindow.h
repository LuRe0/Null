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
		using EnventCallbackFn = std::function<void(const Event&)>;

		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

		bool WindowClosed();
		bool WindowMinimized();

		void InitializeWindowEvents(NEventManager* eventManager);

		// GLFW-specific functions
		static bool InitGLFW();
		GLFWwindow* InitializeWindow(int width, int height, const char* title);
		GLFWwindow* GetWinddow() { return m_Window; };
		static void TerminateGLFW();
		//static void SetFramebufferSizeCallback(GLFWwindow* window, GLFWframebuffersizefun callback);

		static const std::string Name() { return "Window"; }

		unsigned int Height() { return m_Data.m_Height;  }

		unsigned int Width() { return m_Data.m_Width;  }

		void SetVSynch(bool s);

		bool IsVSynch(bool s) { return m_Data.m_VSynch; }

		void SetEventCallback(EnventCallbackFn func) { m_Data.m_callbackFunc = func; };


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

			EnventCallbackFn m_callbackFunc;

			NWindow* m_Parent;
		};

		Window m_Data;


		// GLFW Callback function declarations
		static void WindowResizeCallback(GLFWwindow* window, int width, int height);
		static void WindowCloseCallback(GLFWwindow* window);
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void KeyTypeCallback(GLFWwindow* window, unsigned int chara);
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
		static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	};




}
