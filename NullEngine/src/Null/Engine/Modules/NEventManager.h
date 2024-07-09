#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NEventManager.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

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
		void TriggerEvent(const Event& event);
		void QueueEvent(std::unique_ptr<Event>&& event);
		void DispatchEvents();

		NEventManager() : m_EventId(0) {}

		static const std::string Name() { return "EventManager"; }

	private:
		NEventManager(NEventManager const&) = delete;
		NEventManager& operator=(NEventManager const&) = delete;

		std::vector<std::unique_ptr<Event>> m_EventsQueue;

		std::unordered_map <std::uint32_t, std::vector<std::unique_ptr<IEventHandler>>> m_Subscribers;

		std::uint32_t m_EventId;
	};

}