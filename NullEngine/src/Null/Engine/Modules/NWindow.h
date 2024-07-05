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
#include "GLFW/glfw3.h"


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

		// GLFW-specific functions
		static bool InitGLFW();
		static GLFWwindow* InitializeWindow(int width, int height, const char* title);
		static void TerminateGLFW();
		static void SetFramebufferSizeCallback(GLFWwindow* window, GLFWframebuffersizefun callback);

		static const std::string Name() { return "Window"; }


	private:
		// Private data members
		GLFWwindow* m_Window = nullptr;

		//! Desired window width
		int	m_WinWidth;

		//! Desired window width
		int	m_WinHeight;
	};

}
