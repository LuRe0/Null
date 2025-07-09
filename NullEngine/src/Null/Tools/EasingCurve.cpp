
//------------------------------------------------------------------------------
//
// File Name:	EasingCurve.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "EasingCurve.h"
#include <glad/glad.h>
#include <imgui.h>
#include <magic_enum/magic_enum.hpp>
#include <misc/cpp/imgui_stdlib.h>


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    EasingCurve::EasingCurve() {
        for (int i = 0; i < kCurveSamples; ++i)
            values[i] = float(i) / float(kCurveSamples - 1);
    }
    void EasingCurve::DrawEditorUI(const char* label)
    {
        ImGui::Text("%s", label);

        auto names = magic_enum::enum_names<EasingType>();

        std::vector<std::string> easingNames;
        for (auto name : magic_enum::enum_names<EasingType>()) {
            easingNames.emplace_back(name); // convert string_view → string
        }
        easingNames.pop_back();
        easingNames.emplace_back("Custom");;


        std::vector<const char*> typeNames;
        for (const auto& name : easingNames)
            typeNames.push_back(name.c_str());


        int current = static_cast<int>(type); // your current easing type index

        if (ImGui::Combo("Ease Type", &current, typeNames.data(), static_cast<int>(typeNames.size())))
        {
            if (current >= static_cast<int>(magic_enum::enum_count<EasingType>()))
            {
                // User selected "Custom"
                m_IsCustom = true;
            }
            else
            {
                type = static_cast<EasingType>(current);
                m_IsCustom = false;
                GenerateFromType();
            }
        }

        constexpr float canvasWidth = 300;
        constexpr float canvasHeight = 120;
        ImVec2 canvasSize = ImVec2(canvasWidth, canvasHeight);
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();
        ImVec2 canvasEnd = ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y);

        ImGui::InvisibleButton("##CurveCanvas", canvasSize);
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Draw grid lines
        const int gridX = 10, gridY = 4;
        ImU32 gridColor = IM_COL32(100, 100, 100, 50);
        for (int i = 0; i <= gridX; ++i)
        {
            float x = canvasPos.x + (i / float(gridX)) * canvasSize.x;
            drawList->AddLine(ImVec2(x, canvasPos.y), ImVec2(x, canvasEnd.y), gridColor);
        }
        for (int i = 0; i <= gridY; ++i)
        {
            float y = canvasPos.y + (i / float(gridY)) * canvasSize.y;
            drawList->AddLine(ImVec2(canvasPos.x, y), ImVec2(canvasEnd.x, y), gridColor);
        }

        // Label ticks (Y-axis: value range)
        for (int i = 0; i <= gridY; ++i)
        {
            float value = 0.0f + (1.0f - i / float(gridY)) * (1.0f - 0.0f);
            float y = canvasPos.y + (i / float(gridY)) * canvasSize.y;

            char labelText[16];
            snprintf(labelText, sizeof(labelText), "%.2f", value);
            drawList->AddText(ImVec2(canvasPos.x - 40, y - 8), IM_COL32(255, 255, 255, 255), labelText);
        }

        // Label ticks (X-axis: normalized time)
        const int xLabelStep = 2;  // skip some labels for spacing
        for (int i = 0; i <= gridX; i += xLabelStep)
        {
            float t = i / float(gridX);
            float x = canvasPos.x + t * canvasSize.x;

            char labelText[8];
            snprintf(labelText, sizeof(labelText), "%.1f", t);
            drawList->AddText(ImVec2(x - 10, canvasEnd.y + 4), IM_COL32(255, 255, 255, 255), labelText);
        }

        // Draw curve with clamped normalized values to keep inside canvas
        for (int i = 0; i < kCurveSamples - 1; ++i)
        {
            float v1 = 0.0f + values[i] * (1.0f - 0.0f);
            float v2 = 0.0f + values[i + 1] * (1.0f - 0.0f);

            float normV1 = (v1 - 0.0f) / (1.0f - 0.0f);
            float normV2 = (v2 - 0.0f) / (1.0f - 0.0f);

            normV1 = std::clamp(normV1, 0.0f, 1.0f);
            normV2 = std::clamp(normV2, 0.0f, 1.0f);

            ImVec2 p1 = ImVec2(canvasPos.x + (i / float(kCurveSamples - 1)) * canvasSize.x,
                canvasPos.y + canvasSize.y * (1.0f - normV1));
            ImVec2 p2 = ImVec2(canvasPos.x + ((i + 1) / float(kCurveSamples - 1)) * canvasSize.x,
                canvasPos.y + canvasSize.y * (1.0f - normV2));

            drawList->AddLine(p1, p2, IM_COL32(255, 255, 0, 255), 2.0f);
        }

        // Handle editing with smooth interpolation between points
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            ImVec2 mouse = ImGui::GetIO().MousePos;
            float t = (mouse.x - canvasPos.x) / canvasSize.x;
            t = std::clamp(t, 0.0f, 1.0f);

            float valNorm = 1.0f - (mouse.y - canvasPos.y) / canvasSize.y;
            //valNorm = std::clamp(valNorm, 0.0f, 1.0f);

            float samplePos = t * (kCurveSamples - 1);
            int idxLeft = (int)floor(samplePos);
            int idxRight = idxLeft + 1;
            float localT = samplePos - idxLeft;

            if (idxLeft >= 0 && idxRight < kCurveSamples)
            {
                values[idxLeft] = valNorm * (1.0f - localT) + values[idxLeft] * localT;
                values[idxRight] = valNorm * localT + values[idxRight] * (1.0f - localT);
                m_IsDirty = true;
                type = EasingType::EASINGTYPES;
            }
            else if (idxLeft >= 0 && idxLeft < kCurveSamples)
            {
                values[idxLeft] = valNorm;
                m_IsDirty = true;
                type = EasingType::EASINGTYPES;
            }
        }

        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        // Reset button
        if (ImGui::Button("Reset Curve"))
        {
            for (int i = 0; i < kCurveSamples; ++i)
                values[i] = float(i) / float(kCurveSamples - 1);
            m_IsDirty = true;
            type = EasingType::Linear;
        }
    }

    float EasingCurve::Sample(float t) const
    {
        t = std::clamp(t, 0.0f, 1.0f);
        float samplePos = t * (kCurveSamples - 1);
        int idxLeft = static_cast<int>(samplePos);
        int idxRight = std::min(idxLeft + 1, kCurveSamples - 1);
        float localT = samplePos - idxLeft;

        return std::lerp(values[idxLeft], values[idxRight], localT);
    }

    void EasingCurve::UploadToGPU() {
        if (m_TextureID == 0)
            glGenTextures(1, &m_TextureID);

        glBindTexture(GL_TEXTURE_1D, m_TextureID);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, kCurveSamples, 0, GL_RED, GL_FLOAT, values);

        //if (m_BindlessHandle == 0) {
        //    m_BindlessHandle = glGetTextureHandleARB(m_TextureID);
        //    glMakeTextureHandleResidentARB(m_BindlessHandle);
        //}

        m_IsDirty = false;
    }

    void EasingCurve::GenerateFromType()
    {
        for (int i = 0; i < kCurveSamples; ++i)
        {
            float t = float(i) / (kCurveSamples - 1);
            float eased = Easing::Ease(t, type);
            values[i] = std::clamp(eased, 0.0f, 1.0f);
        }
        m_IsDirty = true;
    }



    void EasingCurve::BindToShader(unsigned int shaderID, const char* uniformName, unsigned int textureUnit)
    {
        if (m_IsDirty)
            UploadToGPU();

        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_1D, m_TextureID);

        GLint location = glGetUniformLocation(shaderID, uniformName);
        glProgramUniform1i(shaderID, location, textureUnit);
    }
}
