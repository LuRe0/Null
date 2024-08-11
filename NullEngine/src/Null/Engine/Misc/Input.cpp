
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
		SUBSCRIBE_INPUT_EVENT(SceneSwitchEvent, &Input::OnSceneSwitched, eventManager);

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

	bool Input::KeyPressed(int keycode)
	{
		return (m_KeyState[keycode] && !m_PrevKeyState[keycode]);
	}

	bool Input::KeyReleased(int keycode)
	{
		return (!m_KeyState[keycode] && m_PrevKeyState[keycode]);
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

	void Input::RegisterToScripAPI(sol::state& lua)
	{
		lua.create_named_table("Input",
			"KeyPressed", &Input::KeyPressed,
			"KeyReleased", &Input::KeyReleased,
			"KeyDown", &Input::KeyDown,
			"KeyTriggered", Input::KeyTriggered,
			"KeyHold", &Input::KeyHold,
			"MousePressed", &Input::MousePressed,
			"MouseReleased", &Input::MouseReleased,
			"MouseTriggered", &Input::MouseTriggered,
			"MouseHold", &Input::MouseHold,
			"GetMouseX", &Input::GetMouseX,
			"GetMouseY", &Input::GetMouseY,
			"GetMousePosition", &Input::GetMouseXY
			// Register other static methods here...
		);


		RegisterGLFWKeys(lua);
	}

	void Input::RegisterGLFWKeys(sol::state& lua)
	{
		// Map GLFW key constants to Lua
		lua["KEY_SPACE"] = GLFW_KEY_SPACE;
		lua["KEY_APOSTROPHE"] = GLFW_KEY_APOSTROPHE;
		lua["KEY_COMMA"] = GLFW_KEY_COMMA;
		lua["KEY_MINUS"] = GLFW_KEY_MINUS;
		lua["KEY_PERIOD"] = GLFW_KEY_PERIOD;
		lua["KEY_SLASH"] = GLFW_KEY_SLASH;
		lua["KEY_0"] = GLFW_KEY_0;
		lua["KEY_1"] = GLFW_KEY_1;
		lua["KEY_2"] = GLFW_KEY_2;
		lua["KEY_3"] = GLFW_KEY_3;
		lua["KEY_4"] = GLFW_KEY_4;
		lua["KEY_5"] = GLFW_KEY_5;
		lua["KEY_6"] = GLFW_KEY_6;
		lua["KEY_7"] = GLFW_KEY_7;
		lua["KEY_8"] = GLFW_KEY_8;
		lua["KEY_9"] = GLFW_KEY_9;
		lua["KEY_SEMICOLON"] = GLFW_KEY_SEMICOLON;
		lua["KEY_EQUAL"] = GLFW_KEY_EQUAL;
		lua["KEY_A"] = GLFW_KEY_A;
		lua["KEY_B"] = GLFW_KEY_B;
		lua["KEY_C"] = GLFW_KEY_C;
		lua["KEY_D"] = GLFW_KEY_D;
		lua["KEY_E"] = GLFW_KEY_E;
		lua["KEY_F"] = GLFW_KEY_F;
		lua["KEY_G"] = GLFW_KEY_G;
		lua["KEY_H"] = GLFW_KEY_H;
		lua["KEY_I"] = GLFW_KEY_I;
		lua["KEY_J"] = GLFW_KEY_J;
		lua["KEY_K"] = GLFW_KEY_K;
		lua["KEY_L"] = GLFW_KEY_L;
		lua["KEY_M"] = GLFW_KEY_M;
		lua["KEY_N"] = GLFW_KEY_N;
		lua["KEY_O"] = GLFW_KEY_O;
		lua["KEY_P"] = GLFW_KEY_P;
		lua["KEY_Q"] = GLFW_KEY_Q;
		lua["KEY_R"] = GLFW_KEY_R;
		lua["KEY_S"] = GLFW_KEY_S;
		lua["KEY_T"] = GLFW_KEY_T;
		lua["KEY_U"] = GLFW_KEY_U;
		lua["KEY_V"] = GLFW_KEY_V;
		lua["KEY_W"] = GLFW_KEY_W;
		lua["KEY_X"] = GLFW_KEY_X;
		lua["KEY_Y"] = GLFW_KEY_Y;
		lua["KEY_Z"] = GLFW_KEY_Z;
		lua["KEY_LEFT_BRACKET"] = GLFW_KEY_LEFT_BRACKET;
		lua["KEY_BACKSLASH"] = GLFW_KEY_BACKSLASH;
		lua["KEY_RIGHT_BRACKET"] = GLFW_KEY_RIGHT_BRACKET;
		lua["KEY_GRAVE_ACCENT"] = GLFW_KEY_GRAVE_ACCENT;
		lua["KEY_WORLD_1"] = GLFW_KEY_WORLD_1;
		lua["KEY_WORLD_2"] = GLFW_KEY_WORLD_2;
		lua["KEY_ESCAPE"] = GLFW_KEY_ESCAPE;
		lua["KEY_ENTER"] = GLFW_KEY_ENTER;
		lua["KEY_TAB"] = GLFW_KEY_TAB;
		lua["KEY_BACKSPACE"] = GLFW_KEY_BACKSPACE;
		lua["KEY_INSERT"] = GLFW_KEY_INSERT;
		lua["KEY_DELETE"] = GLFW_KEY_DELETE;
		lua["KEY_RIGHT"] = GLFW_KEY_RIGHT;
		lua["KEY_LEFT"] = GLFW_KEY_LEFT;
		lua["KEY_DOWN"] = GLFW_KEY_DOWN;
		lua["KEY_UP"] = GLFW_KEY_UP;
		lua["KEY_PAGE_UP"] = GLFW_KEY_PAGE_UP;
		lua["KEY_PAGE_DOWN"] = GLFW_KEY_PAGE_DOWN;
		lua["KEY_HOME"] = GLFW_KEY_HOME;
		lua["KEY_END"] = GLFW_KEY_END;
		lua["KEY_CAPS_LOCK"] = GLFW_KEY_CAPS_LOCK;
		lua["KEY_SCROLL_LOCK"] = GLFW_KEY_SCROLL_LOCK;
		lua["KEY_NUM_LOCK"] = GLFW_KEY_NUM_LOCK;
		lua["KEY_PRINT_SCREEN"] = GLFW_KEY_PRINT_SCREEN;
		lua["KEY_PAUSE"] = GLFW_KEY_PAUSE;
		lua["KEY_F1"] = GLFW_KEY_F1;
		lua["KEY_F2"] = GLFW_KEY_F2;
		lua["KEY_F3"] = GLFW_KEY_F3;
		lua["KEY_F4"] = GLFW_KEY_F4;
		lua["KEY_F5"] = GLFW_KEY_F5;
		lua["KEY_F6"] = GLFW_KEY_F6;
		lua["KEY_F7"] = GLFW_KEY_F7;
		lua["KEY_F8"] = GLFW_KEY_F8;
		lua["KEY_F9"] = GLFW_KEY_F9;
		lua["KEY_F10"] = GLFW_KEY_F10;
		lua["KEY_F11"] = GLFW_KEY_F11;
		lua["KEY_F12"] = GLFW_KEY_F12;
		lua["KEY_F13"] = GLFW_KEY_F13;
		lua["KEY_F14"] = GLFW_KEY_F14;
		lua["KEY_F15"] = GLFW_KEY_F15;
		lua["KEY_F16"] = GLFW_KEY_F16;
		lua["KEY_F17"] = GLFW_KEY_F17;
		lua["KEY_F18"] = GLFW_KEY_F18;
		lua["KEY_F19"] = GLFW_KEY_F19;
		lua["KEY_F20"] = GLFW_KEY_F20;
		lua["KEY_F21"] = GLFW_KEY_F21;
		lua["KEY_F22"] = GLFW_KEY_F22;
		lua["KEY_F23"] = GLFW_KEY_F23;
		lua["KEY_F24"] = GLFW_KEY_F24;
		lua["KEY_F25"] = GLFW_KEY_F25;
		lua["KEY_KP_0"] = GLFW_KEY_KP_0;
		lua["KEY_KP_1"] = GLFW_KEY_KP_1;
		lua["KEY_KP_2"] = GLFW_KEY_KP_2;
		lua["KEY_KP_3"] = GLFW_KEY_KP_3;
		lua["KEY_KP_4"] = GLFW_KEY_KP_4;
		lua["KEY_KP_5"] = GLFW_KEY_KP_5;
		lua["KEY_KP_6"] = GLFW_KEY_KP_6;
		lua["KEY_KP_7"] = GLFW_KEY_KP_7;
		lua["KEY_KP_8"] = GLFW_KEY_KP_8;
		lua["KEY_KP_9"] = GLFW_KEY_KP_9;
		lua["KEY_KP_DECIMAL"] = GLFW_KEY_KP_DECIMAL;
		lua["KEY_KP_DIVIDE"] = GLFW_KEY_KP_DIVIDE;
		lua["KEY_KP_MULTIPLY"] = GLFW_KEY_KP_MULTIPLY;
		lua["KEY_KP_SUBTRACT"] = GLFW_KEY_KP_SUBTRACT;
		lua["KEY_KP_ADD"] = GLFW_KEY_KP_ADD;
		lua["KEY_KP_ENTER"] = GLFW_KEY_KP_ENTER;
		lua["KEY_KP_EQUAL"] = GLFW_KEY_KP_EQUAL;
		lua["KEY_LEFT_SHIFT"] = GLFW_KEY_LEFT_SHIFT;
		lua["KEY_LEFT_CONTROL"] = GLFW_KEY_LEFT_CONTROL;
		lua["KEY_LEFT_ALT"] = GLFW_KEY_LEFT_ALT;
		lua["KEY_LEFT_SUPER"] = GLFW_KEY_LEFT_SUPER;
		lua["KEY_RIGHT_SHIFT"] = GLFW_KEY_RIGHT_SHIFT;
		lua["KEY_RIGHT_CONTROL"] = GLFW_KEY_RIGHT_CONTROL;
		lua["KEY_RIGHT_ALT"] = GLFW_KEY_RIGHT_ALT;
		lua["KEY_RIGHT_SUPER"] = GLFW_KEY_RIGHT_SUPER;
		lua["KEY_MENU"] = GLFW_KEY_MENU;
		lua["KEY_LAST"] = GLFW_KEY_LAST; // Define for completeness
		lua["MOUSE_BUTTON_1"] = GLFW_MOUSE_BUTTON_1; // Define for completeness
		lua["MOUSE_BUTTON_2"] = GLFW_MOUSE_BUTTON_2; // Define for completeness
		lua["MOUSE_BUTTON_3"] = GLFW_MOUSE_BUTTON_3; // Define for completeness
		lua["MOUSE_BUTTON_4"] = GLFW_MOUSE_BUTTON_4; // Define for completeness
		lua["MOUSE_BUTTON_5"] = GLFW_MOUSE_BUTTON_5; // Define for completeness
		lua["MOUSE_BUTTON_6"] = GLFW_MOUSE_BUTTON_6; // Define for completeness
		lua["MOUSE_BUTTON_7"] = GLFW_MOUSE_BUTTON_7; // Define for completeness
		lua["MOUSE_BUTTON_8"] = GLFW_MOUSE_BUTTON_8; // Define for completeness
		lua["MOUSE_BUTTON_LAST"] = GLFW_MOUSE_BUTTON_LAST; // Define for completeness
		lua["MOUSE_BUTTON_LEFT"] = GLFW_MOUSE_BUTTON_LEFT; // Define for completeness
		lua["MOUSE_BUTTON_RIGHT"] = GLFW_MOUSE_BUTTON_RIGHT; // Define for completeness
		lua["MOUSE_BUTTON_MIDDLE"] = GLFW_MOUSE_BUTTON_MIDDLE; // Define for completeness
		
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

	void Input::OnSceneSwitched(const SceneSwitchEvent& event)
	{
		for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) 
		{
			m_KeyState[key] = false;
		}

		for (int button = GLFW_MOUSE_BUTTON_1; button <= GLFW_MOUSE_BUTTON_LAST; ++button) 
		{
			m_MouseState[button] = false;
		}
	}

}
