
//------------------------------------------------------------------------------
//
// File Name:	NThreadPool.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NThreadPool.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE {
    void NThreadPool::Load()
    {
    }
    void NThreadPool::Init()
    {
        m_Terminate = false;
        const uint32_t numThreads = std::thread::hardware_concurrency();

        for (uint32_t i = 0; i < numThreads; ++i) {
            m_Threads.emplace_back(&NThreadPool::ThreadLoop, this);
        }
    }

    void NThreadPool::QueueMainThreadJob(const std::function<void()>& job)
    {
        std::scoped_lock lock(m_MainThreadMutex);
        m_MainThreadQueue.push(job);
    }

    void NThreadPool::QueueMainThreadJob(std::function<void()>&& job)
    {
        std::scoped_lock lock(m_MainThreadMutex);
        m_MainThreadQueue.push(std::move(job));
    }


    void NThreadPool::Update(float dt)
    {
        // Run main-thread jobs
        std::queue<std::function<void()>> queueCopy;
        {
            std::scoped_lock lock(m_MainThreadMutex);
            std::swap(queueCopy, m_MainThreadQueue);
        }

        while (!queueCopy.empty())
        {
            queueCopy.front()();
            queueCopy.pop();
        }
    }

    void NThreadPool::RuntimeUpdate(float dt)
    {
		Update(dt);
    }

    void NThreadPool::Shutdown()
    {
        {
            std::unique_lock lock(m_QueueMutex);
            m_Terminate = true;
        }

        m_Condition.notify_all();

        for (auto& t : m_Threads) {
            if (t.joinable()) t.join();
        }

        m_Threads.clear();
    }

    void NThreadPool::QueueJob(const std::function<void()>& job)
    {
        {
            std::unique_lock lock(m_QueueMutex);
            m_JobQueue.push(job);
            ++m_TotalQueuedJobs;
        }
        m_Condition.notify_one();
    }

    void NThreadPool::QueueJob(std::function<void()>&& job)
    {
        {
            std::unique_lock lock(m_QueueMutex);
            m_JobQueue.push(std::move(job));
            ++m_TotalQueuedJobs;
        }
        m_Condition.notify_one();
    }

    bool NThreadPool::IsBusy() const
    {
        std::scoped_lock lock(m_QueueMutex);
        return !m_JobQueue.empty() || m_ActiveJobs.load() > 0;
    }

    void NThreadPool::ThreadLoop()
    {
        while (true) {
            std::function<void()> job;

            {
                std::unique_lock lock(m_QueueMutex);
                m_Condition.wait(lock, [this] {
                    return m_Terminate || !m_JobQueue.empty();
                    });

                if (m_Terminate && m_JobQueue.empty())
                    return;

                job = std::move(m_JobQueue.front());
                m_JobQueue.pop();
                ++m_ActiveJobs;
            }

            job();

            ++m_TotalCompletedJobs;
            --m_ActiveJobs;
        }
    }

    void NThreadPool::RegisterToScripAPI(sol::state& lua)
    {
    }

    void NThreadPool::RenderImGui()
    {
        //ImGui::Begin("Thread Pool");

        ImGui::Text("Hardware Threads: %d", static_cast<int>(m_Threads.size()));
        ImGui::Text("Jobs in Queue: %d", static_cast<int>(m_JobQueue.size()));
        //ImGui::Text("Main Thread Jobs: %d", static_cast<int>(m_MainThreadQueue.size()));
        size_t mainQueueSize = 0;
        {
            std::scoped_lock lock(m_MainThreadMutex);
            mainQueueSize = m_MainThreadQueue.size();
        }
        ImGui::Text("Main Thread Jobs: %d", static_cast<int>(mainQueueSize));
        ImGui::Text("Active Jobs: %d", m_ActiveJobs.load());
        ImGui::Text("Total Queued: %d", m_TotalQueuedJobs.load());
        ImGui::Text("Total Completed: %d", m_TotalCompletedJobs.load());
        ImGui::Text("Busy: %s", IsBusy() ? "Yes" : "No");

        //ImGui::End();
    }

}
