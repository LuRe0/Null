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
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <atomic>

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
	class NLE_API NThreadPool : public ModuleBase<NThreadPool>
	{
	public:


		void Load() override;
		void Init() override;
		void Update(float dt) override;
		void RuntimeUpdate(float dt) override;
		//void Render() override;
		void RenderLoadScreen();
		void Unload() override;
		void Shutdown() override;

		void QueueJob(const std::function<void()>& job);
		void QueueJob(std::function<void()>&& job);
		bool IsBusy() const;

		bool HasRenderImGui() const override { return true; }

		void RegisterToScripAPI(sol::state& lua) override;

		void RenderImGui() override;


		void QueueMainThreadJob(const std::function<void()>& job);
		void QueueMainThreadJob(std::function<void()>&& job);
	private:

		void ThreadLoop();

		std::queue<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadMutex;

		std::vector<std::thread> m_Threads;
		std::queue<std::function<void()>> m_JobQueue;

		mutable std::mutex m_QueueMutex;
		std::condition_variable m_Condition;

		std::atomic<int> m_ActiveJobs = 0;
		std::atomic<bool> m_Terminate = false;

		// Debug info
		std::atomic<int> m_TotalQueuedJobs = 0;
		std::atomic<int> m_TotalCompletedJobs = 0;

	};

}