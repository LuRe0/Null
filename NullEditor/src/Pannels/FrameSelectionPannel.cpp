
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
#include "FrameSelectionPannel.h"
#include "Null/Engine/Submodules/Scene.h"
//#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>
#include "../Editors/SceneEditor.h"
#include "../../../NullEngine/src/Null/Engine/Submodules/Graphics/Texture.h"
#include "../../../NullEngine/src/Null/Engine/Submodules/Graphics/SpriteSource.h"

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
	//void FrameSelectionPannel::SetContext(Scene* scene)
	//{
	//	m_Context = scene;
	//}

	void FrameSelectionPannel::OnImGUIRender()
	{
		NSpriteSourceManager* srcManager = NSpriteSourceManager::Instance();
		//auto* src = srcManager->Get(m_PannelData->selectedSpriteSourceID);
		ImGui::Begin("Sprite Source Parser");

        // Grid configuration
        ImGui::Text("Grid Configuration:");
        ImGui::InputInt2("Grid Size (Rows, Cols)", &m_GridSize.x);
        ImGui::InputInt2("Cell Size (W, H)", &m_CellSize.x);
        ImGui::InputFloat("Display Size", &m_MaxDisplaySize);
  

        m_GridSize = glm::max(m_GridSize, glm::ivec2(1, 1));
        m_CellSize = glm::max(m_CellSize, glm::ivec2(1, 1));

        ImGui::Separator();

        // Get current sprite source
        //NSpriteSourceManager* srcManager = NSpriteSourceManager::Instance();
        auto* spriteSource = srcManager->Get(m_PannelData->selectedSpriteSourceID);

        if (spriteSource && spriteSource->GetTexture())
        {
            DrawSpriteGrid(spriteSource);

            //ImGui::Text("YA TAH");

        }
        else
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),"No sprite source selected");
        }

		ImGui::End();
	}

    void FrameSelectionPannel::DrawSpriteGrid(SpriteSource* spriteSource)
    {
        auto* texture = spriteSource->GetTexture();
        glm::ivec2 size = texture->GetSize();
        //glm::vec2 dims = spriteSource->GetSize();

        //getting texture size by dividing by spritesource data
        ImVec2 textureSize = { (float)size.x, (float)size.y};
        m_CellSize = glm::ivec2(size.x / m_GridSize.y, size.y / m_GridSize.x);
        // Calculate display size (scale down if too big)
        float maxDisplaySize = m_MaxDisplaySize;
        float scale = std::min(maxDisplaySize / textureSize.x, maxDisplaySize / textureSize.y);
        //scale = std::min(scale, 1.0f); // Don't scale up

        ImVec2 displaySize = { textureSize.x * scale, textureSize.y * scale };
        ImVec2 cellDisplaySize = { m_CellSize.x * scale, m_CellSize.y * scale };

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();
        ImVec2 canvasSize = displaySize;

        // Draw the sprite texture
        ImGui::Image((void*)(intptr_t)texture->GetID(), displaySize, { 0, 0 }, { 1, 1 });

        // Handle mouse interaction for selection
        if (ImGui::IsItemHovered())
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            ImVec2 localPos = { mousePos.x - canvasPos.x, mousePos.y - canvasPos.y };

            // Convert to grid coordinates
            int gridX = (int)(localPos.x / cellDisplaySize.x);
            int gridY = (int)(localPos.y / cellDisplaySize.y);

            // Clamp to valid range
            gridX = std::clamp(gridX, 0, m_GridSize.y); // cols
            gridY = std::clamp(gridY, 0, m_GridSize.x); // rows


            int frameIndex = gridY * m_GridSize.y + gridX;

            if (ImGui::IsMouseClicked(0) && !m_IsSelecting)
            {
                m_SelectedStartFrame = frameIndex;
                m_SelectedEndFrame = frameIndex;
                m_IsSelecting = true;
            }
            else if (ImGui::IsMouseClicked(0) && (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift)))
            {
                m_SelectedEndFrame = frameIndex;
            }
            else if (ImGui::IsMouseClicked(0) && (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)))
            {
                m_SelectedStartFrame = frameIndex;
            }
            else if (ImGui::IsMouseClicked(0) && m_IsSelecting)
            {
                m_SelectedStartFrame = frameIndex;
                m_SelectedEndFrame = frameIndex;
            }
            else if (ImGui::IsMouseDragging(0) && m_IsSelecting)
            {
                m_SelectedEndFrame = frameIndex;
            }
            else if (ImGui::IsMouseReleased(0))
            {
                m_IsSelecting = (m_SelectedStartFrame >= 0);
            }
        }

        // Draw grid lines
        ImU32 gridColor = IM_COL32(255, 255, 255, 100);
        for (int row = 0; row <= m_GridSize.x; ++row)
        {
            float y = canvasPos.y + row * cellDisplaySize.y;
            drawList->AddLine({ canvasPos.x, y }, { canvasPos.x + displaySize.x, y }, gridColor);
        }
        for (int col = 0; col <= m_GridSize.y; ++col)
        {
            float x = canvasPos.x + col * cellDisplaySize.x;
            drawList->AddLine({ x, canvasPos.y }, { x, canvasPos.y + displaySize.y }, gridColor);
        }

        // Draw selection rectangle
        if (m_SelectedStartFrame >= 0)
        {
            int startFrame = std::min(m_SelectedStartFrame, m_SelectedEndFrame);
            int endFrame = std::max(m_SelectedStartFrame, m_SelectedEndFrame);

            ImU32 selectionColor = IM_COL32(0, 255, 0, 100);
            ImU32 borderColor = IM_COL32(0, 255, 0, 255);

            // Highlight each frame in the contiguous sequence
            for (int frame = startFrame; frame <= endFrame; ++frame)
            {
                int row = frame / m_GridSize.y;
                int col = frame % m_GridSize.y;

                // Skip if frame is outside grid bounds
                if (row >= m_GridSize.x || col >= m_GridSize.y) continue;

                ImVec2 rectMin = {
                    canvasPos.x + col * cellDisplaySize.x,
                    canvasPos.y + row * cellDisplaySize.y
                };
                ImVec2 rectMax = {
                    canvasPos.x + (col + 1) * cellDisplaySize.x,
                    canvasPos.y + (row + 1) * cellDisplaySize.y
                };

                drawList->AddRectFilled(rectMin, rectMax, selectionColor);
                drawList->AddRect(rectMin, rectMax, borderColor, 0.0f, 0, 2.0f);
            }
        }

        // Show selection info
        if (m_SelectedStartFrame >= 0)
        {
            int startFrame = std::min(m_SelectedStartFrame, m_SelectedEndFrame);
            int endFrame = std::max(m_SelectedStartFrame, m_SelectedEndFrame);
            int frameCount = endFrame - startFrame + 1;

            ImGui::Text("Selection: Start Frame %d, Count %d", std::min(startFrame, endFrame), frameCount);

            m_PannelData->startFrame = startFrame;
            m_PannelData->endFrame = endFrame;
        }


        if (ImGui::Button("Clear Selection"))
        {
            m_SelectedStartFrame = m_SelectedEndFrame = -1;
        }
    }
    void FrameSelectionPannel::HandleSelection()
    {

    }
}
