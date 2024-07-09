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
		virtual std::string Print() const { return GetName(); }

		virtual ~Event() = default;

	public:
		bool Handled{ false };
	};


}