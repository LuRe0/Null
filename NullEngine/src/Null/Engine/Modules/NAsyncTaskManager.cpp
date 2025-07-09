
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
        int processedThisFrame = 0;
        while (!m_TaskQueue.empty() && processedThisFrame < m_TasksToProcess)
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
                break;
            }

            ++processedThisFrame;
        }
    }

    void NAsyncTaskManager::RenderLoadScreen()
    {
        if (m_TaskQueue.empty())
            return;

        ImGuiIO& io = ImGui::GetIO();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.75f)); // translucent black
        ImGui::Begin("Loading Overlay", nullptr, window_flags);

        ImGui::SetCursorPosY(io.DisplaySize.y * 0.45f);
        ImGui::SetCursorPosX(io.DisplaySize.x * 0.5f - 70);

        ImGui::TextColored(ImVec4(1, 1, 1, 1), "Loading...");

        float progress = (m_TotalTasks > 0) ? (float)m_TasksProcessed / (float)m_TotalTasks : 0.0f;

        ImGui::SetCursorPosY(io.DisplaySize.y * 0.5f);
        ImGui::SetCursorPosX(io.DisplaySize.x * 0.5f - 100);

        ImGui::ProgressBar(progress, ImVec2(200, 20));

        ImGui::End();
        ImGui::PopStyleColor();
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
        ImGui::Begin("Async Task Manager");

        ImGui::Text("Pending Tasks: %d", static_cast<int>(m_TaskQueue.size()));

        ImGui::SliderInt("Tasks Per Frame", &m_TasksToProcess, 1, 20);

        ImGui::TextWrapped("Increase this value to process more tasks per frame, "
            "which speeds up loading but may cause frame drops.");

        ImGui::End();

        RenderLoadScreen();
    }

} // namespace NULLENGINE

