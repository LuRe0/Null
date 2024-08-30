#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	stubfile.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"


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

	class NLE_API Event {
	public:
		enum EventType
		{
			WindowResize, WindowClose, WindowFocus, WindowLostFocus,
			KeyPress, KeyRelease, KeyHold, KeyTyped,
			MouseMove, MouseButtonPress, MouseButtonRelease, MouseButtonHold, MouseScrolled,
			EntityCreated, EntityDestroyed,EntityAddComponent, EntityRemoveComponent,
			SceneSwitch,
			InitializeBox2D,
			ScriptCreated, ScriptRemoved, ScriptModified,
			EngineRunState, EngineEditState,
			EntityParented, EntitySeparated,
			CollisionEnter, CollisionExit,
		};

		virtual std::unique_ptr<Event> Clone() const = 0;
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string Print() const { return GetName(); }

		virtual ~Event() = default;

	public:
		bool Handled{ false };
	};


}