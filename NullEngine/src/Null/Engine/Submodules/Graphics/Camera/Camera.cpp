
//------------------------------------------------------------------------------
//
// File Name:	Camera.cm_PPSettings
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Camera.h"




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
    Camera::Frustum Camera::Frustum::FromCamera(glm::vec3 position, glm::vec3 up, glm::vec3 right, glm::vec3 front, float aspect, float fovY, float zNear, float zFar)
    {
        Frustum frustum;

        const float halfVSide = zFar * 0.75f * tanf(fovY * 0.5f);
        const float halfHSide = halfVSide * aspect;
        const glm::vec3 frontMultFar = zFar * front;

        frustum.planes[0] = { position + zNear * front, front };                        // Near
        frustum.planes[1] = { position + frontMultFar,      -front };                       // Far
        frustum.planes[2] = { position, glm::normalize(glm::cross(frontMultFar - right * halfHSide, up)) };       // right
        frustum.planes[3] = { position, glm::normalize(glm::cross(up, frontMultFar + right * halfHSide)) };       // Left
        frustum.planes[4] = { position, glm::normalize(glm::cross(right, frontMultFar - up * halfVSide)) };       // Top
        frustum.planes[5] = { position, glm::normalize(glm::cross(frontMultFar + up * halfVSide, right)) };       // Bottom

        return frustum;
    }
    void Camera::View()
    {
        if (ImGui::TreeNode("Post Process"))
        {
            ImGui::Checkbox("Enabled", &m_PPSettings.Enabled);

            if (m_PPSettings.Enabled)
            {
                if (ImGui::CollapsingHeader("Bloom", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Checkbox("Use Bloom", &m_PPSettings.UseBloom);
                    if (m_PPSettings.UseBloom)
                    {
                        ImGui::SliderFloat("Bloom Threshold", &m_PPSettings.BloomThreshold, 0.0f, 5.0f);
                        ImGui::SliderFloat("Bloom Intensity", &m_PPSettings.BloomIntensity, 0.0f, 2.0f);
                    }
                }

                if (ImGui::CollapsingHeader("Vignette"))
                {
                    ImGui::Checkbox("Use Vignette", &m_PPSettings.UseVignette);
                    if (m_PPSettings.UseVignette)
                    {
                        ImGui::SliderFloat("Vignette Radius", &m_PPSettings.VignetteRadius, 0.0f, 1.0f);
                        ImGui::SliderFloat("Vignette Intensity", &m_PPSettings.VignetteIntensity, 0.0f, 1.0f);
                    }
                }

                if (ImGui::CollapsingHeader("Grayscale"))
                {
                    ImGui::Checkbox("Use Grayscale", &m_PPSettings.UseGrayscale);
                    if (m_PPSettings.UseGrayscale)
                    {
                        ImGui::SliderFloat("Grayscale Amount", &m_PPSettings.GrayscaleAmount, 0.0f, 1.0f);
                    }
                }

                if (ImGui::CollapsingHeader("Tint"))
                {
                    ImGui::Checkbox("Use Tint", &m_PPSettings.UseTint);
                    if (m_PPSettings.UseTint)
                    {
                        ImGui::ColorEdit4("Tint Color", glm::value_ptr(m_PPSettings.TintColor));
                        ImGui::SliderFloat("Tint Strength", &m_PPSettings.TintStrength, 0.0f, 1.0f);
                    }
                }

                if (ImGui::CollapsingHeader("Chromatic Aberration"))
                {
                    ImGui::Checkbox("Use Chromatic Aberration", &m_PPSettings.UseChromatic);
                    if (m_PPSettings.UseChromatic)
                    {
                        ImGui::SliderFloat("Chromatic Offset", &m_PPSettings.ChromaticOffset, 0.0f, 5.0f);
                    }
                }

                if (ImGui::CollapsingHeader("Film Grain"))
                {
                    ImGui::Checkbox("Use Grain", &m_PPSettings.UseGrain);
                    if (m_PPSettings.UseGrain)
                    {
                        ImGui::SliderFloat("Grain Amount", &m_PPSettings.GrainAmount, 0.0f, 1.0f);
                    }
                }
            }
            ImGui::TreePop();
        }
    }
}
