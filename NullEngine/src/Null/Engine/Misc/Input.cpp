
//------------------------------------------------------------------------------
//
// File Name:	NStubModule.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Input.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//


#define SUBSCRIBE_INPUT_EVENT(EVENT_TYPE, EVENT_CALLBACK, EVENT_MANAGER) \
    { \
        EventCallback<EVENT_TYPE> callback = std::bind(EVENT_CALLBACK, std::placeholders::_1); \
        std::unique_ptr<IEventHandler> handler = std::make_unique<EventHandler<EVENT_TYPE>>(callback, EVENT_TYPE::GetStaticEventType()); \
        EVENT_MANAGER->Subscribe(EVENT_TYPE::GetStaticEventType(), std::move(handler)); \
    }

namespace NULLENGINE
{
	glm::vec2 Input::m_MousePos(0.0f, 0.0f);
	std::unordered_map<int, bool> Input::m_KeyState;
	std::unordered_map<int, bool> Input::m_PrevKeyState;
	std::unordered_map<int, bool> Input::m_KeyRepeatState;
	std::unordered_map<int, bool> Input::m_MouseState;
	std::unordered_map<int, bool> Input::m_PrevMouseState;

	void Input::Init()
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();


		SUBSCRIBE_INPUT_EVENT(KeyPressEvent, &Input::OnKey, eventManager);
		SUBSCRIBE_INPUT_EVENT(KeyReleaseEvent, &Input::OnKey, eventManager);
		SUBSCRIBE_INPUT_EVENT(KeyHoldEvent, &Input::OnKey, eventManager);
		SUBSCRIBE_INPUT_EVENT(MouseButtonPressEvent, &Input::OnMouseButton, eventManager);
		SUBSCRIBE_INPUT_EVENT(MouseButtonReleaseEvent, &Input::OnMouseButton, eventManager);
		SUBSCRIBE_INPUT_EVENT(MouseMoveEvent, &Input::OnMouseMoved, eventManager);

		//SUBSCRIBE_EVENT(WindowCloseEvent, &ImGuiLayer::OnWindowClose, eventManager);
		//SUBSCRIBE_EVENT(KeyPressEvent, &ImGuiLayer::OnKeyPressed, eventManager);
		//SUBSCRIBE_EVENT(KeyTypedEvent, &ImGuiLayer::OnKeyTyped, eventManager);
		//SUBSCRIBE_EVENT(KeyReleaseEvent, &ImGuiLayer::OnKeyReleased, eventManager);
	}

	void Input::Update()
	{
		// Copy current state to previous state
		for (auto& key : m_KeyState)
		{
			m_PrevKeyState[key.first] = key.second;
		}
		for (auto& button : m_MouseState)
		{
			m_PrevMouseState[button.first] = button.second;
		}

		m_KeyRepeatState.clear();
	}

	bool Input::KeyPressed(int keycode, int keycode2)
	{
		return (m_KeyState[keycode] && !m_PrevKeyState[keycode]) ||
			(keycode2 != -1 && m_KeyState[keycode2] && !m_PrevKeyState[keycode2]);
	}

	bool Input::KeyReleased(int keycode, int keycode2)
	{
		return (!m_KeyState[keycode] && m_PrevKeyState[keycode]) ||
			(keycode2 != -1 && !m_KeyState[keycode2] && m_PrevKeyState[keycode2]);
	}

	bool Input::KeyDown(int keycode)
	{
		return m_KeyState[keycode];
	}

	bool Input::KeyTriggered(int keycode)
	{
		return m_KeyState[keycode] && !m_PrevKeyState[keycode];
	}

	bool Input::KeyHold(int keycode)
	{
		return m_KeyState[keycode] && m_KeyRepeatState[keycode];
	}

	bool Input::MousePressed(int button)
	{
		return m_MouseState[button] && !m_PrevMouseState[button];
	}

	bool Input::MouseReleased(int button)
	{
		return !m_MouseState[button] && m_PrevMouseState[button];
	}

	bool Input::MouseTriggered(int button)
	{
		return m_MouseState[button] && !m_PrevMouseState[button];
	}

	bool Input::MouseHold(int button)
	{
		return m_MouseState[button] && m_PrevMouseState[button];
	}

	float Input::GetMouseX()
	{
		return GetMouseXY().x;
	}
	float Input::GetMouseY()
	{
		return  GetMouseXY().y;
	}

	glm::vec2 Input::GetMouseXY()
	{
		return m_MousePos;
	}

	//void Input::SetKeyState(int key, int action)
	//{
	//	if (action == GLFW_PRESS)
	//	{
	//		m_KeyState[key] = true;
	//	}
	//	else if (action == GLFW_RELEASE)
	//	{
	//		m_KeyState[key] = false;
	//	}
	//	else if (action == GLFW_REPEAT)
	//	{
	//		m_KeyRepeatState[key] = true;
	//	}
	//}
	//void Input::SetMouseState(int button, int action)
	//{
	//	if (action == GLFW_PRESS)
	//		m_MouseState[button] = true;
	//	else if (action == GLFW_RELEASE)
	//		m_MouseState[button] = false;
	//}

	//void Input::SetMousePos(double xpos, double ypos)
	//{
	//	m_MousePos.x = xpos;
	//	m_MousePos.y = ypos;
	//}

	void Input::OnEvent(const Event& e)
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		eventManager->QueueEvent(std::move(e.Clone()));
	}

	void Input::OnKey(const KeyEvent& e)
	{

		int key = e.GetKeyCode();
		switch (e.GetEventType())
		{
		case Event::EventType::KeyPress:
		{
			m_KeyState[key] = true;
		}
			break;
		case Event::EventType::KeyRelease:
		{
			m_KeyState[key] = false;
		}
		break;
		case Event::EventType::KeyHold:
		{
			m_KeyRepeatState[key] = true;
		}
		break;
		default:
			break;
		}



	}

	void Input::OnMouseButton(const MouseButtonEvent& e)
	{
		int button = e.GetButton();
		switch (e.GetEventType())
		{
		case Event::EventType::MouseButtonPress:
		{
			m_MouseState[button] = true;
		}
		break;
		case Event::EventType::MouseButtonRelease:
		{
			m_MouseState[button] = false;
		}
		break;
		default:
			break;
		}

	}

	void Input::OnMouseMoved(const MouseMoveEvent& e)
	{
		m_MousePos.x = e.GetX();
		m_MousePos.y = e.GetY();
	}

}
