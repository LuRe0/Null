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
	//! Wrapper class for delta Input
	class NLE_API Input
	{
	public:


		//! Initialize class
		static Input& Instance()
		{
			static Input instance;
			return instance;
		}

		//! Update current delta Input
		static void Init();

		//! Update current delta Input
		static void Update();

		static bool KeyPressed(int keycode);
		static bool KeyReleased(int keycode);

		static bool KeyDown(int keycode);
		static bool KeyTriggered(int keycode);
		static bool KeyHold(int keycode);

		static bool MousePressed(int button);
		static bool MouseReleased(int button);
		static bool MouseTriggered(int button);
		static bool MouseHold(int button);

		static float GetMouseX();
		static float GetMouseY();

		static glm::vec2 GetMouseXY();

		static void RegisterToScripAPI(sol::state& lua);




		//void SetKeyState(int key, int action);
		//void SetMouseState(int button, int action);
		//void SetMousePos(double xpos, double ypos);


		void OnEvent(const Event& e);

	
		static void OnKey(const KeyEvent& event);
		//void OnKeyHold(const KeyHoldEvent& event);
		static void OnMouseButton(const MouseButtonEvent& event);
		static void OnMouseMoved(const MouseMoveEvent& event);
		static void OnSceneSwitched(const SceneSwitchEvent& event);

	private:
		// Private constructor and destructor
		Input() {}
		~Input() {}

		static glm::vec2 m_MousePos;
		static std::unordered_map<int, bool> m_KeyState;
		static std::unordered_map<int, bool> m_PrevKeyState;
		static std::unordered_map<int, bool> m_KeyRepeatState;
		static std::unordered_map<int, bool> m_MouseState;
		static std::unordered_map<int, bool> m_PrevMouseState;

		static void RegisterGLFWKeys(sol::state& lua);

	};

}