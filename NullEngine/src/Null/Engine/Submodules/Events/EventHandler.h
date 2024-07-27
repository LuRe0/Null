#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	EventHandler.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Submodules/Events/IEvents.h"



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
	template<typename T>
	using EventCallback = std::function<void(const T& e)>;
	
	enum EventPriority 
	{
		Low,
		Medium,
		High
	};



	class IEventHandler {
	public:
		void Exec(const Event& e)
		{
			Call(e);
		}

		virtual Event::EventType GetType() const = 0;
		virtual EventPriority GetPriority() const = 0;

		virtual ~IEventHandler() = default;

	private:
		virtual void Call(const Event& e) = 0;
	};

	template<typename T>
	class EventHandler : public IEventHandler {
	public:
		explicit EventHandler(const EventCallback<T>& handler, Event::EventType type, EventPriority priority = Low)
			: m_handler(handler)
			, m_handlerType(type)
			, m_Priority(priority) {};

	private:
		void Call(const Event& e) override
		{
			if (e.GetEventType() == T::GetStaticEventType()) {
				m_handler(static_cast<const T&>(e));
			}
		}

		EventHandler(EventHandler const&) = delete;
		EventHandler& operator=(EventHandler const&) = delete;

		Event::EventType GetType() const override { return m_handlerType; }
		EventPriority GetPriority() const override { return m_Priority; }

		EventCallback<T> m_handler;
		Event::EventType m_handlerType;
		EventPriority m_Priority;
	};

}