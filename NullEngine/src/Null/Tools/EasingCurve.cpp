
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

        static const char* typeNames[] = {
                "Custom",
                "Linear",
                "Ease In",
                "Ease Out",
                "Ease In-Out",
                "Fast In",
                "Fast Out",
                "InOut Peak",
                "Parabola",
                "SmoothStep",
                "SmootherStep",
                "Elastic Out",
                "Bounce Out",
                "Back In",
                "Back Out"
        };

        int current = static_cast<int>(type);
        if (ImGui::Combo("Ease Type", &current, typeNames, IM_ARRAYSIZE(typeNames)))
            type = static_cast<CurveType>(current);

        if (type != CurveType::Custom)
        {
            if (ImGui::Button("Apply Easing"))
                GenerateFromType();
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
                type = CurveType::Custom;
            }
            else if (idxLeft >= 0 && idxLeft < kCurveSamples)
            {
                values[idxLeft] = valNorm;
                m_IsDirty = true;
                type = CurveType::Custom;
            }
        }

        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        // Reset button
        if (ImGui::Button("Reset Curve"))
        {
            for (int i = 0; i < kCurveSamples; ++i)
                values[i] = float(i) / float(kCurveSamples - 1);
            m_IsDirty = true;
            type = CurveType::Custom;
        }
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
            float eased = 0.0f;

            switch (type)
            {
            case CurveType::Linear:      eased = t; break;
            case CurveType::EaseIn:      eased = std::sqrt(t); break;
            case CurveType::EaseOut:     eased = t * t; break;
            case CurveType::FastIn:      eased = std::sqrt(std::sqrt(t)); break;
            case CurveType::FastOut:     eased = std::pow(t, 4.0f); break;
            case CurveType::EaseInOut:
                if (t < 0.5f)
                    eased = std::pow(t * 2.0f, 2.0f) * 0.5f;
                else
                    eased = std::sqrt((t - 0.5f) * 2.0f) * 0.5f + 0.5f;
                break;
            case CurveType::InOutPeak:
                eased = (t < 0.5f) ? 2.0f * t : 2.0f * (1.0f - t);
                break;

            case CurveType::Parabola:
                eased = 4.0f * t * (1.0f - t); // Peaks at t=0.5
                break;

            case CurveType::SmoothStep:
                eased = t * t * (3.0f - 2.0f * t);
                break;

            case CurveType::SmootherStep:
                eased = t * t * t * (t * (6.0f * t - 15.0f) + 10.0f);
                break;

            case CurveType::ElasticOut:
            {
                float c4 = (2.0f * 3.14159265f) / 3.0f;
                eased = (t == 0.0f) ? 0.0f :
                    (t == 1.0f) ? 1.0f :
                    pow(2.0f, -10.0f * t) * sin((t * 10.0f - 0.75f) * c4) + 1.0f;
            }
            break;

            case CurveType::BounceOut:
                if (t < 1 / 2.75f) {
                    eased = 7.5625f * t * t;
                }
                else if (t < 2 / 2.75f) {
                    t -= 1.5f / 2.75f;
                    eased = 7.5625f * t * t + 0.75f;
                }
                else if (t < 2.5f / 2.75f) {
                    t -= 2.25f / 2.75f;
                    eased = 7.5625f * t * t + 0.9375f;
                }
                else {
                    t -= 2.625f / 2.75f;
                    eased = 7.5625f * t * t + 0.984375f;
                }
                break;

            case CurveType::BackIn:
            {
                const float c1 = 1.70158f;
                eased = c1 * t * t * t - c1 * t * t;
            }
            break;

            case CurveType::BackOut:
            {
                const float c1 = 1.70158f;
                const float c3 = c1 + 1.0f;
                eased = 1.0f + c3 * pow(t - 1.0f, 3) + c1 * pow(t - 1.0f, 2);
            }
            break;

            default: return;
            }

            values[i] = std::lerp(0.0f, 1.0f, eased);
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
