
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
#include "Null/Tools/ImGuiH.h"
#include "NIncludes.h"



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
		if (auto [open, enabled] = ImGuiH::DrawTopLevelHeader("Post Process", &m_PPSettings.Enabled, 1000); open)
		{
			if (enabled)
			{
				if (auto [open, enabled] = ImGuiH::DrawModifierHeader("Bloom", &m_PPSettings.UseBloom, 0); open)
				{
					if (enabled)
					{
						ImGui::SliderFloat("Bloom Threshold", &m_PPSettings.BloomThreshold, 0.0f, 5.0f);
						ImGui::SliderFloat("Bloom Intensity", &m_PPSettings.BloomIntensity, 0.0f, 2.0f);
					}
					ImGui::TreePop();
				}

				if (auto [open, enabled] = ImGuiH::DrawModifierHeader("Vignette", &m_PPSettings.UseVignette, 1); open)
				{
					if (enabled)
					{
						ImGui::SliderFloat("Vignette Radius", &m_PPSettings.VignetteRadius, 0.0f, 1.0f);
						ImGui::SliderFloat("Vignette Intensity", &m_PPSettings.VignetteIntensity, 0.0f, 1.0f);
					}
					ImGui::TreePop();
				}

				if (auto [open, enabled] = ImGuiH::DrawModifierHeader("Grayscale", &m_PPSettings.UseGrayscale, 2); open)
				{
					if (enabled)
					{
						ImGui::SliderFloat("Grayscale Amount", &m_PPSettings.GrayscaleAmount, 0.0f, 1.0f);
					}
					ImGui::TreePop();
				}

				if (auto [open, enabled] = ImGuiH::DrawModifierHeader("Tint", &m_PPSettings.UseTint, 3); open)
				{
					if (enabled)
					{
						ImGui::ColorEdit4("Tint Color", glm::value_ptr(m_PPSettings.TintColor));
						ImGui::SliderFloat("Tint Strength", &m_PPSettings.TintStrength, 0.0f, 1.0f);
					}
					ImGui::TreePop();
				}

				if (auto [open, enabled] = ImGuiH::DrawModifierHeader("Chromatic Aberration", &m_PPSettings.UseChromatic, 4); open)
				{
					if (enabled)
					{
						ImGui::SliderFloat("Chromatic Offset", &m_PPSettings.ChromaticOffset, 0.0f, 5.0f);
					}
					ImGui::TreePop();
				}

				if (auto [open, enabled] = ImGuiH::DrawModifierHeader("Film Grain", &m_PPSettings.UseGrain, 5); open)
				{
					if (enabled)
					{
						ImGui::SliderFloat("Grain Amount", &m_PPSettings.GrainAmount, 0.0f, 1.0f);
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
	void Camera::Write(JSON& json)
	{
		if (!m_PPTemplateFilename.empty())
			json["ppTemplateName"] = m_PPTemplateFilename;
		else
			m_PPSettings.Serialize(json);
	}
	void Camera::SetShakeOffset(const glm::vec3& offset)
	{
		if (offset != m_ShakeOffset)
		{
			m_ShakeOffset = offset; m_IsDirty = true;
		}
	}
}
