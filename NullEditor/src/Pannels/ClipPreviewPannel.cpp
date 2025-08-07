
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
#include "ClipPreviewPannel.h"
#include "Null/Engine/Submodules/Scene.h"
//#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>
#include "../Editors/SceneEditor.h"
#include "../../../NullEngine/src/Null/Tools/ImGuiH.h"
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
	//void ClipPreviewPannel::SetContext(Scene* scene)
	//{
	//	m_Context = scene;
	//}

	void ClipPreviewPannel::OnImGUIRender()
	{
        NSpriteSourceManager* srcManager = NSpriteSourceManager::Instance();
        auto* src = srcManager->Get(m_PannelData->selectedSpriteSourceID);
		ImGui::Begin("Preview Clip");

        //this is the main rendering function
        RenderAnimationClipPreview(*m_PannelData, src);

		ImGui::End();
	}


    void ClipPreviewPannel::RenderAnimationClipPreview(AnimationPannelData& animData, SpriteSource* spriteSource)
    {
        //simple check for spritesources
        if (!spriteSource || !spriteSource->GetTexture()) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),"No sprite source for animation preview");
            return;
        }

        auto* texture = spriteSource->GetTexture();
        glm::ivec2 textureSize = texture->GetSize();
        auto& clip = animData.workingClip;

        //check for having frames selected, although it is nevet zero because of the +1 will need to fix that
        if (clip.frameCount == 0) 
        {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Select frames to preview animation");
            return;
        }

        // Update animation timing
        if (animData.isPlaying)
        {
            m_PlaybackTime += ImGui::GetIO().DeltaTime;

            // Calculate behavior based on flags
            bool isLooping = clip.flags.IsSet(AnimationFlags_IsLooping);
            bool isPingPong = clip.flags.IsSet(AnimationFlags_IsPingPong);

            if (isPingPong)
            {
                // Ping-pong: play forward then backward
                float totalDuration = (clip.frameCount * 2 - 2) * clip.frameDuration; // -2 because first/last frame aren't repeated

                //i dont like that it overwrites my playback time
                if (m_PlaybackTime >= totalDuration)
                {
                    if (isLooping) 
                    {
                        m_PlaybackTime = 0.0f; // Reset for loop
                    }
                    else {
                        m_PlaybackTime = totalDuration;
                        animData.isPlaying = false; // Stop at end
                    }
                }
            }
            else
            {
                // Normal linear playback
                float totalDuration = clip.frameCount * clip.frameDuration;

                if (m_PlaybackTime >= totalDuration)
                {
                    if (isLooping) {
                        m_PlaybackTime = 0.0f; // Reset for loop
                    }
                    else {
                        m_PlaybackTime = totalDuration;
                        animData.isPlaying = false; // Stop at end
                    }
                }
            }
        }

        // Calculate current frame based on flags
        int currentFrame = GetCurrentAnimationFrame(clip, m_PlaybackTime);

        // Convert frame index to grid coordinates
        int row = currentFrame / animData.m_GridSize.y;
        int col = currentFrame % animData.m_GridSize.y;

        // Calculate UV coordinates
        float u0 = (float)(col * animData.m_CellSize.x) / textureSize.x;
        float v0 = (float)(row * animData.m_CellSize.y) / textureSize.y;
        float u1 = (float)((col + 1) * animData.m_CellSize.x) / textureSize.x;
        float v1 = (float)((row + 1) * animData.m_CellSize.y) / textureSize.y;

        // Display the current frame with a border to show it's playing
        ImVec2 displaySize(animData.m_CellSize.x * 2.0f, animData.m_CellSize.y * 2.0f); // 3x scale for preview

        ImGui::Text("Animation Preview:");

        // Add a colored border when playing
        if (animData.isPlaying) {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 imagePos = ImGui::GetCursorScreenPos();
            ImU32 borderColor = IM_COL32(0, 255, 0, 255); // Green when playing
            drawList->AddRect(imagePos, ImVec2(imagePos.x + displaySize.x, imagePos.y + displaySize.y), borderColor, 0.0f, 0, 3.0f);
        }

        ImGui::Image((void*)(intptr_t)texture->GetID(), displaySize,
            ImVec2(u0, v0), ImVec2(u1, v1));

        // Animation controls
        ImGui::Separator();

        // Play/Pause button with different behavior based on flags
        const char* playButtonText = animData.isPlaying ? "Pause##preview" : "Play##preview";
        if (ImGui::Button(playButtonText, ImVec2(60, 0)))
        {
            animData.isPlaying = !animData.isPlaying;
        }

        ImGui::SameLine();
        if (ImGui::Button("Stop##preview", ImVec2(60, 0)))
        {
            animData.isPlaying = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Reset##preview", ImVec2(60, 0)))
        {
            m_PlaybackTime = 0.0f;
        }

        // Playback speed control
        ImGui::SameLine();
        static float playbackSpeed = 1.0f;
        ImGui::SetNextItemWidth(100);
        if (ImGui::SliderFloat("Speed##preview", &playbackSpeed, 0.1f, 3.0f, "%.1fx"))
        {
            // You could modify the effective frame duration here if needed
        }

        // Progress bar
        float progress = GetAnimationProgress(clip, m_PlaybackTime);
        ImGui::ProgressBar(progress, ImVec2(-1, 0), "");

        // Frame and timing info
        ImGui::Text("Frame: %d/%d (Grid: %d,%d)",
            currentFrame, clip.startingFrame + clip.frameCount - 1, col, row);

        // Show current playback state
        std::string stateText = "State: ";
        if (!animData.isPlaying) {
            stateText += "Stopped";
        }
        else {
            stateText += "Playing";
            if (clip.flags.IsSet(AnimationFlags_IsReversed)) stateText += " (Reversed)";
            if (clip.flags.IsSet(AnimationFlags_IsPingPong)) stateText += " (Ping-Pong)";
            if (clip.flags.IsSet(AnimationFlags_IsLooping)) stateText += " (Looping)";
        }
        ImGui::Text("%s", stateText.c_str());

        ImGui::Text("Time: %.3fs / %.3fs", animData.animLength,
            GetTotalAnimationDuration(clip));
    }


    int ClipPreviewPannel::GetCurrentAnimationFrame(const AnimationClip& clip, float playbackTime)
    {
        bool isReversed = clip.flags.IsSet(AnimationFlags_IsReversed);
        bool isPingPong = clip.flags.IsSet(AnimationFlags_IsPingPong);


        if (isPingPong)
        {
            // Ping-pong logic
            float halfCycleDuration = (clip.frameCount - 1) * clip.frameDuration;
            float cycleTime = fmod(playbackTime, halfCycleDuration * 2);

            if (cycleTime <= halfCycleDuration)
            {
                // Forward phase
                int frameOffset = (int)(cycleTime / clip.frameDuration);
                frameOffset = std::min(frameOffset, (int)clip.frameCount - 1);
                return isReversed ?
                    clip.startingFrame + clip.frameCount - 1 - frameOffset :
                    clip.startingFrame + frameOffset;
            }
            else
            {
                // Backward phase
                float backwardTime = cycleTime - halfCycleDuration;
                int frameOffset = (int)(backwardTime / clip.frameDuration);
                frameOffset = std::min(frameOffset, (int)clip.frameCount - 1);
                return isReversed ?
                    clip.startingFrame + frameOffset :
                    clip.startingFrame + clip.frameCount - 1 - frameOffset;
            }
        }
        else
        {
            // Normal linear playback
            int frameOffset = (int)(playbackTime / clip.frameDuration);
            frameOffset = frameOffset % clip.frameCount;

            return isReversed ?
                clip.startingFrame + clip.frameCount - 1 - frameOffset :
                clip.startingFrame + frameOffset;
        }
    }


    float ClipPreviewPannel::GetAnimationProgress(const AnimationClip& clip, float playbackTime)
    {
        float totalDuration = GetTotalAnimationDuration(clip);
        if (totalDuration <= 0) return 0.0f;

        return std::min(1.0f, playbackTime / totalDuration);
    }


    float ClipPreviewPannel::GetTotalAnimationDuration(const AnimationClip& clip)
    {
        bool isPingPong = clip.flags.IsSet(AnimationFlags_IsPingPong);

        if (isPingPong) {
            return (clip.frameCount * 2 - 2) * clip.frameDuration;
        }
        else {
            return clip.frameCount * clip.frameDuration;
        }
    }
}
