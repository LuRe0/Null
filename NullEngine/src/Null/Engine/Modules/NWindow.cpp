#include "stdafx.h"
#include "NWindow.h"
#include "Null/Tools/Trace.h"
#include "Null/Engine/Modules/NEventManager.h"
#include "Null/Engine/Submodules/Events/IEvents.h"


#include <glad/glad.h>

namespace NULLENGINE
{

#define SUBSCRIBE_EVENT(EVENT_TYPE, EVENT_CALLBACK, EVENT_MANAGER) \
    { \
        EventCallback<EVENT_TYPE> callback = std::bind(EVENT_CALLBACK, this, std::placeholders::_1); \
        std::unique_ptr<IEventHandler> handler = std::make_unique<EventHandler<EVENT_TYPE>>(callback, EVENT_TYPE::GetStaticEventType()); \
        EVENT_MANAGER->Subscribe(EVENT_TYPE::GetStaticEventType(), std::move(handler)); \
    }

    void NWindow::Load()
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

    void NWindow::Init()
    {
        NEventManager* eventManager = m_Parent->Get<NEventManager>();

        InitializeWindowEvents(eventManager);



        // Set GLFW callbacks
        glfwSetWindowSizeCallback(m_Window, WindowResizeCallback);
        glfwSetWindowCloseCallback(m_Window, WindowCloseCallback);
        glfwSetKeyCallback(m_Window, KeyCallback);
        glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);
        glfwSetCursorPosCallback(m_Window, MouseMoveCallback);
        glfwSetScrollCallback(m_Window, MouseScrollCallback);

    }

    void NWindow::Update(float dt)
    {
        if (m_Window)
        {
            // Update loop: render, poll events, etc.
            glfwSwapBuffers(m_Window);

            //setWindowResizeCallback([](int width, int height) {
            //    TriggerEvent(WindowResizeEvent(width, height));
            //    })

            //    setWindowCloseCallback(m_window, [](GLFWwindow* window) {
            //        UniquePtr<WindowCloseEvent> closeEvent = std::move(MakeUniquePtr<WindowCloseEvent>());
            //        QueueEvent(std:move(closeEvent));
            //        });

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

    void NWindow::InitializeWindowEvents(NEventManager* eventManager)
    {
        //EventCallback<WindowResizeEvent> callback = std::bind(&NWindow::OnWindowResize, this, std::placeholders::_1);;

        //std::unique_ptr<IEventHandler> handler = std::make_unique<EventHandler<WindowResizeEvent>>(callback, Event::WindowResize);
        //eventManager->Subscribe(WindowResizeEvent::GetStaticEventType(), std::move(handler));


        SUBSCRIBE_EVENT(WindowResizeEvent, &NWindow::OnWindowResize, eventManager);
        SUBSCRIBE_EVENT(WindowCloseEvent, &NWindow::OnWindowClose, eventManager);
        SUBSCRIBE_EVENT(KeyPressEvent, &NWindow::OnKey, eventManager);
        SUBSCRIBE_EVENT(KeyHoldEvent, &NWindow::OnKey, eventManager);
        SUBSCRIBE_EVENT(KeyReleaseEvent, &NWindow::OnKey, eventManager);
        SUBSCRIBE_EVENT(MouseButtonPressEvent, &NWindow::OnMouseButton, eventManager);
        SUBSCRIBE_EVENT(MouseButtonReleaseEvent, &NWindow::OnMouseButton, eventManager);
        SUBSCRIBE_EVENT(MouseMoveEvent, &NWindow::OnMouseMove, eventManager);
        SUBSCRIBE_EVENT(MouseScrolledEvent, &NWindow::OnMouseScroll, eventManager);
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


        m_Data.m_Height = 800;
        m_Data.m_Width = 600;
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

    //void NWindow::SetFramebufferSizeCallback(GLFWwindow* window, GLFWframebuffersizefun callback)
    //{
    //    glfwSetFramebufferSizeCallback(window, callback);
    //}

    void NWindow::ResizeWindowCallback(const WindowResizeEvent& event)
    {
        const WindowResizeEvent& resizeEvent = dynamic_cast<const WindowResizeEvent&>(event);

        NLE_TRACE("Changing Window size to ({0}, {1})", resizeEvent.GetWidth(), resizeEvent.GetHeight());
    }



    ///GLFW Callbacks
    void  NWindow::WindowResizeCallback(GLFWwindow* window, int width, int height)
    {
        Window& data = *((Window*)glfwGetWindowUserPointer(window));

        data.m_Width = width;
        data.m_Height = height;

        NEventManager* eventManager = data.m_Parent->GetParent()->Get<NEventManager>();

        eventManager->TriggerEvent(WindowResizeEvent(width, height));
    }

    void  NWindow::WindowCloseCallback(GLFWwindow* window)
    {
        Window& data = *((Window*)glfwGetWindowUserPointer(window));

        NEventManager* eventManager = data.m_Parent->GetParent()->Get<NEventManager>();

        eventManager->TriggerEvent(WindowCloseEvent());
    }

    void  NWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        switch (action)
        {
        case GLFW_PRESS:
        {

            Window& data = *((Window*)glfwGetWindowUserPointer(window));

            NEventManager* eventManager = data.m_Parent->GetParent()->Get<NEventManager>();

            eventManager->TriggerEvent(KeyPressEvent(key, 0));
            break;
        }
        case GLFW_RELEASE:
        {

            Window& data = *((Window*)glfwGetWindowUserPointer(window));

            NEventManager* eventManager = data.m_Parent->GetParent()->Get<NEventManager>();

            eventManager->TriggerEvent(KeyReleaseEvent(key));
            break;
        }
        case GLFW_REPEAT:
        {

            Window& data = *((Window*)glfwGetWindowUserPointer(window));

            NEventManager* eventManager = data.m_Parent->GetParent()->Get<NEventManager>();

            eventManager->TriggerEvent(KeyHoldEvent(key));
            break;
        }
        default:
            break;
        }
    }

    void  NWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        switch (action)
        {
        case GLFW_PRESS:
        {

            Window& data = *((Window*)glfwGetWindowUserPointer(window));

            NEventManager* eventManager = data.m_Parent->GetParent()->Get<NEventManager>();

            eventManager->TriggerEvent(MouseButtonPressEvent(button));
            break;
        }
        case GLFW_RELEASE:
        {

            Window& data = *((Window*)glfwGetWindowUserPointer(window));

            NEventManager* eventManager = data.m_Parent->GetParent()->Get<NEventManager>();

            eventManager->TriggerEvent(MouseButtonReleaseEvent(button));
            break;
        }
        default:
            break;
        }
    }

    void  NWindow::MouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {

        Window& data = *((Window*)glfwGetWindowUserPointer(window));

        NEventManager* eventManager = data.m_Parent->GetParent()->Get<NEventManager>();

        eventManager->TriggerEvent(MouseMoveEvent(static_cast<float>(xpos), static_cast<float>(ypos)));
    }

    void  NWindow::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {

        Window& data = *((Window*)glfwGetWindowUserPointer(window));

        NEventManager* eventManager = data.m_Parent->GetParent()->Get<NEventManager>();

         eventManager->TriggerEvent(MouseScrolledEvent(static_cast<float>(xoffset), static_cast<float>(yoffset)));
    }

    void NWindow::OnWindowResize(const WindowResizeEvent& e)
    {
        NLE_CORE_INFO("{0}", e.Print());
    }

    void NWindow::OnWindowClose(const WindowCloseEvent& e)
    {
        NLE_CORE_INFO("{0}", e.Print());
    }

    void NWindow::OnKey(const KeyEvent& e)
    {
        NLE_CORE_INFO("{0}", e.Print());

    }

    void NWindow::OnMouseButton(const MouseButtonEvent& e)
    {
        NLE_CORE_INFO("{0}", e.Print());
    }

    void NWindow::OnMouseMove(const MouseMoveEvent& e)
    {
        NLE_CORE_INFO("{0}", e.Print());

    }

    void NWindow::OnMouseScroll(const MouseScrolledEvent& e)
    {
        NLE_CORE_INFO("{0}", e.Print());
    }
}
