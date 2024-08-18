#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NEventManager.h
// Author(s):	name
// 
//------------------------------------------------------------------------------
/*
Code adapted from https://denyskryvytskyi.github.io/event-system
*/
//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
#include "Null/Engine/Submodules/Events/EventHandler.h"


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
	class NLE_API NEventManager : public IModule
	{
	public:

		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;
		void RuntimeUpdate(float dt) override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

		std::uint32_t GenerateID();

		/// <summary>
		/// this registers an event to the manager
		/// </summary>
		/// <param name="eventId"></param>
		/// <param name="handler"></param>
		void Subscribe(std::uint32_t, std::unique_ptr<IEventHandler>&& handler);

		/// <summary>
		/// removes an event from the manager
		/// </summary>
		/// <param name="eventId"></param>
		/// <param name="handlerName"></param>
		void Unsubscribe(std::uint32_t, Event::EventType handlerType);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="event"></param>
		bool TriggerEvent(const Event& event);
		void QueueEvent(std::unique_ptr<Event>&& event);

		/// <summary>
		/// Use to add events to queue from within an event.
		/// </summary>
		void QueueAsync(std::unique_ptr<Event>&& event);
		void DispatchEvents();

		NEventManager() : m_EventId(0) {}


		void RegisterToScripAPI(sol::state& lua) override;

		static const std::string Name() { return "EventManager"; }

	private:
		NEventManager(NEventManager const&) = delete;
		NEventManager& operator=(NEventManager const&) = delete;

		std::vector<std::unique_ptr<Event>> m_EventsQueue;
		std::vector<std::unique_ptr<Event>> m_AsychEventsQueue;

		std::unordered_map <std::uint32_t, std::vector<std::unique_ptr<IEventHandler>>> m_Subscribers;

		EventCallback<Event> mEvent;

		std::uint32_t m_EventId;

		struct EventHandlerPrioritySorter {
			bool operator()(const std::unique_ptr<IEventHandler>& lhs, const std::unique_ptr<IEventHandler>& rhs) const {
				return lhs->GetPriority() > rhs->GetPriority(); // Higher priority comes first
			}
		};

		void SortSubscribers(std::uint32_t eventId);
	};

#define SUBSCRIBE_EVENT(EVENT_TYPE, EVENT_CALLBACK, EVENT_MANAGER, PRIORITY) \
    { \
        EventCallback<EVENT_TYPE> callback = std::bind(EVENT_CALLBACK, this, std::placeholders::_1); \
        std::unique_ptr<IEventHandler> handler = std::make_unique<EventHandler<EVENT_TYPE>>(callback, EVENT_TYPE::GetStaticEventType(), PRIORITY); \
        EVENT_MANAGER->Subscribe(EVENT_TYPE::GetStaticEventType(), std::move(handler)); \
    }

}