
//------------------------------------------------------------------------------
//
// File Name:	NStub.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "TimelinePannel.h"
#include "Null/Engine/Submodules/Scene.h"
//#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>
#include "NIncludes.h"

//#include "backends/imgui_impl_opengl3.h"
//#include "backends/imgui_impl_glfw.h"

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void TimelinePannel::OnImGUIRender()
	{
		const auto& modules = NEngine::Instance().GetModules();

		ImGui::Begin("Timeline");

		RenderMontageTimeline(*m_PannelData);

		ImGui::End();
	}


    void TimelinePannel::RenderMontageTimeline(MontagePannelData& panelData)
    {
        if (!panelData.selectedClip)
        {
            ImGui::TextColored(ImVec4(1,0,0,1), "No animation clip selected");
            return;
        }

        AnimationClip* clip = panelData.selectedClip;

        // Timeline settings
        const float frameWidth = 40.0f;
        const float frameHeight = 30.0f;
        const float trackHeight = 25.0f;
        const int maxVisibleFrames = 20; // Adjust based on panel width

        ImGui::Text("Clip: %s (Frames: %d)", "ClipName", clip->frameCount); // TODO: Get actual name


        // Begin timeline area
        ImGui::BeginChild("Timeline", ImVec2(0, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();
        ImVec2 canvasSize = ImGui::GetContentRegionAvail();

        // Calculate total width of all frames
        float trackWidth = canvasSize.x * 0.75f; // 90% of available width
        float frameWidthDynamic = trackWidth / clip->frameCount;


        float startX = canvasPos.x + (canvasSize.x - trackWidth) * 0.5f;

        for (uint32_t frame = 0; frame < clip->frameCount; ++frame)
        {
            ImU32 borderColor = frame == m_PannelData->currentFrame && m_PannelData->isPlaying ?
                IM_COL32(0, 255, 0, 255) : IM_COL32(100, 100, 100, 255);
            ImVec2 framePos(startX + frame * frameWidthDynamic, canvasPos.y);
            ImVec2 frameEnd(framePos.x + frameWidthDynamic, framePos.y + frameHeight);

            // Frame box background
            ImU32 frameColor = (frame % 2 == 0) ? IM_COL32(60, 60, 60, 255) : IM_COL32(80, 80, 80, 255);
            drawList->AddRectFilled(framePos, frameEnd, frameColor);
            drawList->AddRect(framePos, frameEnd, borderColor);

            // Frame number text
            char frameText[8];
            sprintf(frameText, "%d", frame);
            ImVec2 textSize = ImGui::CalcTextSize(frameText);
            ImVec2 textPos(framePos.x + (frameWidthDynamic - textSize.x) * 0.5f,
                framePos.y + (frameHeight - textSize.y) * 0.5f);
            drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), frameText);

            bool isHovering = ImGui::IsMouseHoveringRect(framePos, frameEnd);

            if (isHovering)
            {
                drawList->AddRect(framePos, frameEnd, IM_COL32(255, 255, 0, 255));

                // Only process mouse events when hovering over this specific frame
                if (ImGui::IsMouseClicked(0) && !m_IsSelecting)
                {
                    m_SelectedStartFrame = frame;
                    m_SelectedEndFrame = frame;
                    m_IsSelecting = true;
                }
                else if (ImGui::IsMouseClicked(0) && (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift)))
                {
                    m_SelectedEndFrame = frame;
                }
                else if (ImGui::IsMouseClicked(0) && (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)))
                {
                    m_SelectedStartFrame = frame;
                }
                else if (ImGui::IsMouseClicked(0) && m_IsSelecting)
                {
                    m_SelectedStartFrame = frame;
                    m_SelectedEndFrame = frame;
                }
                else if (ImGui::IsMouseDragging(0) && m_IsSelecting)
                {
                    m_SelectedEndFrame = frame;
                }
            }
            else if (frame >= m_SelectedStartFrame && frame <= m_SelectedEndFrame)
            {
                drawList->AddRect(framePos, frameEnd, IM_COL32(0, 255, 255, 255));
            }

            // Handle mouse release globally (outside the hover check)
            if (ImGui::IsMouseReleased(0))
            {
                m_IsSelecting = false; // or whatever logic you need here
            }
        }

        // Move cursor down for event tracks
        ImGui::SetCursorScreenPos(ImVec2(canvasPos.x, canvasPos.y + frameHeight + 5));

        // Draw event tracks
        const int maxTracks = 5;

        // Calculate centered start/end positions
        float startTrackX = canvasPos.x + (canvasSize.x - trackWidth) * 0.5f;
        float endTrackX = startTrackX + trackWidth;


        for (int track = 0; track < maxTracks; ++track)
        {
            ImVec2 trackPos(startTrackX, canvasPos.y + frameHeight + 10 + track * (trackHeight + 5));
            ImVec2 trackEnd(endTrackX, trackPos.y + trackHeight);

            drawList->AddRectFilled(trackPos, trackEnd, IM_COL32(40, 40, 40, 255));
            drawList->AddRect(trackPos, trackEnd, IM_COL32(60, 60, 60, 255));

            // Draw events for this track
            for (uint32_t i = 0; i < clip->eventCount; ++i)
            {
                const EventPlacement& event = clip->eventPlacements[i];
                if (event.eventAssetID == 0) continue; // Skip empty slots

                // For now, put all events on track 0 (we'll improve this later)
                if (track != 0) continue;

                ImVec2 eventStart(canvasPos.x + event.frame * frameWidth, trackPos.y + 2);

                if (event.endFrame == event.frame)
                {
                    // Instant event - draw as vertical line
                    ImVec2 eventEnd(eventStart.x + 3, trackPos.y + trackHeight - 2);
                    drawList->AddRectFilled(eventStart, eventEnd, IM_COL32(255, 100, 100, 255));
                }
                else
                {
                    // Window event - draw as horizontal bar
                    ImVec2 eventEnd(canvasPos.x + (event.endFrame + 1) * frameWidth, trackPos.y + trackHeight - 2);
                    drawList->AddRectFilled(eventStart, eventEnd, IM_COL32(100, 255, 100, 255));
                }
            }

            // Invisible button for adding events to this track
            ImGui::SetCursorScreenPos(trackPos);
            ImGui::InvisibleButton(("Track" + std::to_string(track)).c_str(),
                ImVec2(trackWidth, trackHeight));

            if (ImGui::IsItemClicked())
            {
                // Calculate which frame was clicked
                ImVec2 mousePos = ImGui::GetMousePos();
                int clickedFrame = (int)((mousePos.x - canvasPos.x) / trackWidth);
                if (clickedFrame >= 0 && clickedFrame < (int)clip->frameCount)
                {
                    // TODO: Add event at this frame
                    ImGui::OpenPopup("AddEvent");
                }
            }
        }

        // Add event popup
        if (ImGui::BeginPopup("AddEvent"))
        {
            if (ImGui::MenuItem("Add Instant Event"))
            {
                // TODO: Add instant event logic
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::MenuItem("Add Window Event"))
            {
                // TODO: Add window event logic
                ImGui::CloseCurrentPopup();
            }

			ImGui::Separator();

            const auto& names = NScriptingInterface::Instance()->GetScriptNames();
            if (ImGui::BeginMenu("Add Event"))
            {

                static ImGuiTextFilter filter;

                filter.Draw("##eventsearchbarBehAdd");

                ImGui::Separator();

                auto names = NAnimationEventManager::Instance()->GetResourceNames();


                for (auto name : names)
                {
                    if (filter.PassFilter(name.c_str()))
                    {
                        if (ImGui::MenuItem(name.c_str()))
                        {

                        }
                    }
                }


				ImGui::Separator();

                if (ImGui::MenuItem("New Event Trigger"))
                {
                    m_ShowCreationMenu = true;
                }

                if (ImGui::MenuItem("New Event Window"))
                {
                    m_ShowCreationMenu = true;
                }


                ImGui::EndMenu();
            }

            ImGui::EndPopup();
        }

        // Set the content size for scrolling
        ImGui::SetCursorScreenPos(ImVec2(canvasPos.x, canvasPos.y + frameHeight + maxTracks * (trackHeight + 5) + 20));
        ImGui::Dummy(ImVec2(clip->frameCount * frameWidth, 1));

        ImGui::EndChild();
    }
}
