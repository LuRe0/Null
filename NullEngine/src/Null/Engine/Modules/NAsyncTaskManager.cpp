
//------------------------------------------------------------------------------
//
// File Name:	NStubModule.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NAsyncTaskManager.h"
#include <mutex> // optional if you want thread safety later


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//


namespace NULLENGINE
{

    void NAsyncTaskManager::Load()
    {
        // Initialize any resources if needed
    }

    void NAsyncTaskManager::Init()
    {
        // Called once after Load(), setup if needed
    }


    void NAsyncTaskManager::Update(float dt)
    {
        using clock = std::chrono::steady_clock;
        auto start = clock::now();

        const float maxTimePerFrameMS = m_MaxTimePerFrameMS;

        while (!m_TaskQueue.empty())
        {
            auto& task = m_TaskQueue.front();
            task->Process();

            if (task->IsComplete())
            {
                m_TaskQueue.pop();
                ++m_TasksProcessed;
            }
            else
            {
                break; // let it continue next frame
            }

            auto now = clock::now();
            float elapsedMS = std::chrono::duration<float, std::milli>(now - start).count();
            if (elapsedMS > maxTimePerFrameMS)
                break; // Don't blow the frame time
        }
    }


    void NAsyncTaskManager::RuntimeUpdate(float dt)
    {
        Update(dt);
    }

    //void NAsyncTaskManager::Render()
    //{
    //}

    void NAsyncTaskManager::RenderLoadScreen()
    {
        bool show = !m_TaskQueue.empty() || m_TasksProcessed < m_TotalTasks;
        if (!show)
            return;

        ImGuiIO& io = ImGui::GetIO();
        const float windowHeight = 80.0f;
        const float windowWidth = io.DisplaySize.x;

        // Position it at the bottom of the screen
        ImGui::SetNextWindowPos(ImVec2(0.0f, io.DisplaySize.y - windowHeight), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Always);

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoScrollbar;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.85f)); // Dark translucent background

        if (ImGui::Begin("Bottom Loading Bar", nullptr, flags))
        {
            float progress = (m_TotalTasks > 0) ? (float)m_TasksProcessed / (float)m_TotalTasks : 0.0f;

            ImGui::SetCursorPosX((windowWidth - 200.0f) * 0.5f);
            ImGui::ProgressBar(progress, ImVec2(200, 20));

            ImGui::SetCursorPosY(50.0f);
            ImGui::SetCursorPosX((windowWidth - 200.0f) * 0.5f);
            if (progress >= 1.0f)
                ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Loading Complete!");
            else
                ImGui::TextColored(ImVec4(1, 1, 1, 1), "Loading...");
        }

        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    void NThreadPool::Unload()
    {
    }


    void NAsyncTaskManager::RegisterToScripAPI(sol::state& lua)
    {
    }


    void NAsyncTaskManager::Unload()
    {
        while (!m_TaskQueue.empty())
            m_TaskQueue.pop();
    }

    void NAsyncTaskManager::Shutdown()
    {
        // Final shutdown cleanup
        Unload();
    }

    void NAsyncTaskManager::EnqueueTask(std::unique_ptr<IAsyncTask> task)
    {

        if (m_TaskQueue.empty())
        {
            m_TotalTasks = 0;
            m_TasksProcessed = 0;
        }

        if (task)
        {
            m_TaskQueue.push(std::move(task));
            ++m_TotalTasks;
        }
    }

    bool NAsyncTaskManager::HasPendingTasks() const
    {
        return !m_TaskQueue.empty();
    }

    void NAsyncTaskManager::RenderImGui()
    {
        //ImGui::Begin("Async Task Manager");

        ImGui::Text("Pending Tasks: %d", static_cast<int>(m_TaskQueue.size()));

        ImGui::SliderFloat("Max Time Per Frame (ms)", &m_MaxTimePerFrameMS, 0.1f, 10.0f);

        ImGui::TextWrapped("Increase this value to process more tasks per frame, "
            "which speeds up loading but may cause frame drops.");

        //ImGui::End();

        RenderLoadScreen();
    }

} // namespace NULLENGINE

