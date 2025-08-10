
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
#include "MontagePreviewPannel.h"
#include "Null/Engine/Submodules/Scene.h"
//#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>
#include "NIncludes.h"
#include "../Editors/AnimationMontageEditor.h"

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
	void MontagePreviewPannel::OnImGUIRender()
	{
		const auto& modules = NEngine::Instance().GetModules();

		ImGui::Begin("Montage Preview");

		m_Parent->HandleFileDragDrop();

		RenderAnimationClip(m_PannelData->selectedSpriteSource);
	
		ImGui::End();
	}


    void MontagePreviewPannel::RenderAnimationClip(SpriteSource* spriteSource)
    {
        //simple check for spritesources
        if (!spriteSource || !spriteSource->GetTexture())
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No sprite source for animation preview");
            return;
        }

        ImVec2 availableRegion = ImGui::GetContentRegionAvail();

        float buttonHeight = ImGui::GetFrameHeight();
        float buttonSpacing = ImGui::GetStyle().ItemSpacing.y;
        float reservedHeight = buttonHeight + buttonSpacing;

        ImVec2 canvasSize = ImVec2(availableRegion.x, availableRegion.y - reservedHeight);


        glm::ivec2 gridSize = { spriteSource->GetRows(), spriteSource->GetCols() };

        auto* texture = spriteSource->GetTexture();
        glm::ivec2 textureSize = texture->GetSize();
        auto& clip = *m_PannelData->selectedClip;


        ImVec2 size = { (float)textureSize.x, (float)textureSize.y };
        glm::ivec2 cellSize = glm::ivec2(textureSize.x / gridSize.y, textureSize.y / gridSize.x);


        const float frameDuration = clip.animationLength / clip.frameCount;

        //check for having frames selected, although it is nevet zero because of the +1 will need to fix that
        if (clip.frameCount == 0)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Select frames to preview animation");
            return;
        }

        // Update animation timing
        if (m_PannelData->isPlaying)
        {
            m_PlaybackTime += ImGui::GetIO().DeltaTime;

            // Calculate behavior based on flags
            bool isLooping = clip.flags.IsSet(AnimationFlags_IsLooping);
            bool isPingPong = clip.flags.IsSet(AnimationFlags_IsPingPong);

            if (isPingPong)
            {
                // Ping-pong: play forward then backward
                float totalDuration = (clip.frameCount * 2 - 2) * frameDuration; // -2 because first/last frame aren't repeated

                //i dont like that it overwrites my playback time
                if (m_PlaybackTime >= totalDuration)
                {
                    if (isLooping)
                    {
                        m_PlaybackTime = 0.0f; // Reset for loop
                    }
                    else {
                        m_PlaybackTime = totalDuration;
                        m_PannelData->isPlaying = false; // Stop at end
                    }
                }
            }
            else
            {
                // Normal linear playback
                float totalDuration = clip.animationLength;

                if (m_PlaybackTime >= totalDuration)
                {
                    if (isLooping) {
                        m_PlaybackTime = 0.0f; // Reset for loop
                    }
                    else {
                        m_PlaybackTime = totalDuration;
                        m_PannelData->isPlaying = false; // Stop at end
                    }
                }
            }
        }

        // Calculate current frame based on flags
        int currentFrame = GetCurrentAnimationFrame(clip, m_PlaybackTime);

        // Convert frame index to grid coordinates
        int row = currentFrame / gridSize.y;
        int col = currentFrame % gridSize.y;

        // Calculate UV coordinates
        float u0 = (float)(col * cellSize.x) / textureSize.x;
        float v0 = (float)(row * cellSize.y) / textureSize.y;
        float u1 = (float)((col + 1) * cellSize.x) / textureSize.x;
        float v1 = (float)((row + 1) * cellSize.y) / textureSize.y;


        // Calculate aspect ratios
        float imageAspect = textureSize.x / textureSize.y;
        float canvasAspect = canvasSize.x / canvasSize.y;

        // Calculate the display size that fits within canvas while maintaining aspect ratio
        ImVec2 displaySize;
        ImVec2 displayPos;

        if (imageAspect > canvasAspect) {
            displaySize.x = canvasSize.x;
            displaySize.y = canvasSize.x / imageAspect;

            displayPos.x = 0;
            displayPos.y = (canvasSize.y - displaySize.y) * 0.5f;
        }
        else
        {
            displaySize.y = canvasSize.y;
            displaySize.x = canvasSize.y * imageAspect;

            displayPos.x = (canvasSize.x - displaySize.x) * 0.5f;
            displayPos.y = 0;
        }

        ImVec2 canvasPos = ImGui::GetCursorScreenPos(); // For current cursor position

        ImVec2 finalPos = ImVec2(canvasPos.x + displayPos.x, canvasPos.y + displayPos.y);


        // Add a colored border when playing
        if (m_PannelData->isPlaying) {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 imagePos = ImGui::GetCursorScreenPos();
            ImU32 borderColor = IM_COL32(0, 255, 0, 255); // Green when playing
            drawList->AddRect(finalPos, ImVec2(finalPos.x + displaySize.x, finalPos.y + displaySize.y), borderColor, 0.0f, 0, 3.0f);
        }


        ImGui::SetCursorScreenPos(finalPos);
        ImGui::Image((void*)(intptr_t)texture->GetID(), displaySize,
            ImVec2(u0, v0), ImVec2(u1, v1));

        ImGui::Separator();

        ImVec2 buttonAreaStart = ImVec2(canvasPos.x, canvasPos.y + canvasSize.y + buttonSpacing);
        ImGui::SetCursorScreenPos(buttonAreaStart);

        // Center the buttons horizontally
        float totalButtonWidth = 60 * 3 + ImGui::GetStyle().ItemSpacing.x * 2; // 3 buttons + 2 spacings
        float startX = (availableRegion.x - totalButtonWidth) * 0.5f;

        ImGui::SetCursorPosX(startX);

        const char* playButtonText = m_PannelData->isPlaying ? "Pause##preview" : "Play##preview";
        if (ImGui::Button(playButtonText, ImVec2(60, 0)))
        {
            m_PannelData->isPlaying = !m_PannelData->isPlaying;
        }

        ImGui::SameLine();
        if (ImGui::Button("Stop##preview", ImVec2(60, 0)))
        {
            m_PannelData->isPlaying = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Reset##preview", ImVec2(60, 0)))
        {
            m_PannelData->m_PlaybackTime = 0.0f;
			m_PannelData->isPlaying = false;
			m_PannelData->currentFrame = currentFrame = 0;
        }

        // Animation controls



        //// Frame and timing info
        //ImGui::Text("Frame: %d/%d (Grid: %d,%d)",
        //    currentFrame, clip.startingFrame + clip.frameCount - 1, col, row);

        //// Show current playback state
        //std::string stateText = "State: ";
        //if (!m_PannelData->isPlaying) {
        //    stateText += "Stopped";
        //}
        //else {
        //    stateText += "Playing";
        //    if (clip.flags.IsSet(AnimationFlags_IsReversed)) stateText += " (Reversed)";
        //    if (clip.flags.IsSet(AnimationFlags_IsPingPong)) stateText += " (Ping-Pong)";
        //    if (clip.flags.IsSet(AnimationFlags_IsLooping)) stateText += " (Looping)";
        //}
        //ImGui::Text("%s", stateText.c_str());

        //ImGui::Text("Time: %.3fs / %.3fs", frameDuration,
        //    GetTotalAnimationDuration(clip));

		m_PannelData->currentFrame = currentFrame;
    }


    int MontagePreviewPannel::GetCurrentAnimationFrame(const AnimationClip& clip, float playbackTime)
    {
        bool isReversed = clip.flags.IsSet(AnimationFlags_IsReversed);
        bool isPingPong = clip.flags.IsSet(AnimationFlags_IsPingPong);
        const float frameDuration = clip.animationLength / clip.frameCount;


        if (isPingPong)
        {
            // Ping-pong logic
            float halfCycleDuration = (clip.frameCount - 1) * frameDuration;
            float cycleTime = fmod(playbackTime, halfCycleDuration * 2);

            if (cycleTime <= halfCycleDuration)
            {
                // Forward phase
                int frameOffset = (int)(cycleTime / frameDuration);
                frameOffset = std::min(frameOffset, (int)clip.frameCount - 1);
                return isReversed ?
                    clip.startingFrame + clip.frameCount - 1 - frameOffset :
                    clip.startingFrame + frameOffset;
            }
            else
            {
                // Backward phase
                float backwardTime = cycleTime - halfCycleDuration;
                int frameOffset = (int)(backwardTime / frameDuration);
                frameOffset = std::min(frameOffset, (int)clip.frameCount - 1);
                return isReversed ?
                    clip.startingFrame + frameOffset :
                    clip.startingFrame + clip.frameCount - 1 - frameOffset;
            }
        }
        else
        {
            // Normal linear playback
            int frameOffset = (int)(playbackTime / frameDuration);
            frameOffset = frameOffset % clip.frameCount;

            return isReversed ?
                clip.startingFrame + clip.frameCount - 1 - frameOffset :
                clip.startingFrame + frameOffset;
        }
    }


}
