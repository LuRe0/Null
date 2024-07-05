#include "stdafx.h"
#include "NWindow.h"
#include "Null/Tools/Trace.h"




void NULLENGINE::NWindow::Load()
{
}

void NULLENGINE::NWindow::Init()
{
    if (!InitGLFW()) {
        NLE_ERROR("Failed to initialize GLFW");
    }
    else {
        m_Window = InitializeWindow(800, 600, "Null Engine Window");
        if (!m_Window) {
            NLE_ERROR("Failed to create GLFW window");
        }
        else {
            //SetFramebufferSizeCallback(window, framebuffer_size_callback);
            NLE_TRACE("Creating Window {0} ({1}, {2})", "Null Engine Window", 800, 600);
        }
    }
}

void NULLENGINE::NWindow::Update(float dt)
{
    if (m_Window)
    {
        // Update loop: render, poll events, etc.
        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }
}

void NULLENGINE::NWindow::Unload()
{

}

void NULLENGINE::NWindow::Shutdown()
{
    TerminateGLFW();
}

bool NULLENGINE::NWindow::WindowClosed()
{
    return !m_Window || glfwWindowShouldClose(m_Window);
}

bool NULLENGINE::NWindow::InitGLFW()
{
    if (!glfwInit()) {
        NLE_ERROR("Failed to create GLFW window");
        return false;
    }

    return true;
}

GLFWwindow* NULLENGINE::NWindow::InitializeWindow(int width, int height, const char* title)
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

    glfwMakeContextCurrent(window);
    return window;
}

void NULLENGINE::NWindow::TerminateGLFW()
{
}

void NULLENGINE::NWindow::SetFramebufferSizeCallback(GLFWwindow* window, GLFWframebuffersizefun callback)
{
    glfwSetFramebufferSizeCallback(window, callback);
}
