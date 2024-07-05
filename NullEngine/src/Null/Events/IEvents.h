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
			KeyPress, KeyRelease, KeyHold, 
			MouseMove, MouseButtonPress, MouseButtonRelease, MouseButtonHold, MouseScrolled,
		};

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual void Trace() = 0;

	private:
		bool m_Handled = false;

		friend class EventDispatcher;
	};

	class NLE_API EventListener {
	public:
		virtual void OnEvent(Event& event) = 0;
	};

	class NLE_API EventDispatcher {
	public:
		void Dispatch(Event& event) {
			for (auto& listener : listeners) {
				listener->OnEvent(event);
			}
		}

		void AddListener(EventListener* listener) {
			listeners.push_back(listener);
		}

	private:
		std::vector<EventListener*> listeners;
	};

}