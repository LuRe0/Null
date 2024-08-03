
//------------------------------------------------------------------------------
//
// File Name:	NEventManager.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NEventManager.h"
#include "Null/Tools/Trace.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void NEventManager::Load()
	{
	}

	void NEventManager::Init()
	{
	}

	void NEventManager::Update(float dt)
	{
		DispatchEvents();
	}

	void NEventManager::Unload()
	{
	}

	void NEventManager::Shutdown()
	{
		m_EventsQueue.clear();
	}

	std::uint32_t NEventManager::GenerateID()
	{
		return ++m_EventId;
	}

	void NEventManager::Subscribe(std::uint32_t eventId, std::unique_ptr<IEventHandler>&& handler)
	{

		auto subscribers = m_Subscribers.find(eventId);
		if (subscribers != m_Subscribers.end()) {
			auto& handlers = subscribers->second;


			handlers.emplace_back(std::move(handler));  // Move the handler into the vector
			SortSubscribers(eventId);

		}
		else {

			auto& handlers = m_Subscribers[eventId];

			handlers.emplace_back(std::move(handler));
			SortSubscribers(eventId);

		}
	}

	void NEventManager::Unsubscribe(std::uint32_t eventId, Event::EventType handlerType)
	{
		auto& handlers = m_Subscribers[eventId];
		for (auto it = handlers.begin(); it != handlers.end(); ) {
			if ((*it)->GetType() == handlerType) {
				it = handlers.erase(it);  // `erase` returns the next iterator
			}
			else {
				++it;  // increment the iterator only if no erasure
			}
		}
	}
	void NEventManager::TriggerEvent(const Event& event_)
	{
		for (auto& handler : m_Subscribers[event_.GetEventType()]) {
			handler->Exec(event_);
		}
	}

	void NEventManager::QueueEvent(std::unique_ptr<Event>&& event)
	{
		m_EventsQueue.emplace_back(std::move(event));
	}


	void NEventManager::DispatchEvents()
	{
		for (auto eventIt = m_EventsQueue.begin(); eventIt != m_EventsQueue.end();) {
			if (!eventIt->get()->Handled) {
				TriggerEvent(*eventIt->get());
				eventIt = m_EventsQueue.erase(eventIt);
			}
			else {
				++eventIt;
			}
		}
	}
	void NEventManager::RegisterToScripAPI(sol::state& lua)
	{
		//lua.new_usertype<NEventManager>(
		//	"Window",
		//	sol::constructors<NEventManager(int)>(),  // Constructor
		//	"Quit", &NEventManager::
		//);

		// Expose the existing instance to Lua under a different global variable
		//lua[Module<NEventManager>::TypeName().c_str()] = this;
	}
	void NEventManager::SortSubscribers(std::uint32_t eventId)
	{
		auto& handlers = m_Subscribers[eventId];
		std::sort(handlers.begin(), handlers.end(), EventHandlerPrioritySorter());
	}
}