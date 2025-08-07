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
#include "AnimationClipEditor.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "Null/Tools/FileDialog.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <ImGuizmo/ImGuizmo.h>
#include <../../Box2D/include/box2d/box2d.h>

#include <magic_enum/magic_enum.hpp>

#include "../../../NullEngine/src/Null/Engine/Submodules/Scene.h"
#include "../../../NullEngine/src/Null/Tools/GLMHelper.h"

#include "Null/Engine/Submodules/Graphics/Camera/Controllers/OrthographicCameraController.h"
#include "Null/Engine/Submodules/Graphics/Camera/Controllers/PerspectiveCameraController.h"

#include "../Pannels/SpriteSourcePannel.h"
#include "../Pannels/FrameSelectionPannel.h"
#include "../Pannels/ClipPropertiesPannel.h"
#include "../Pannels/ClipPreviewPannel.h"
#include "NIncludes.h"


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	AnimationClipEditor::AnimationClipEditor() : ImGuiEditor("Animation Clip", EditorType::ANIMATION_CLIP)
	{
		m_IsOpen = false; // Scene editor always open by default

		OnAttach();
	}

	void AnimationClipEditor::OnAttach()
	{
		auto pannel = std::make_unique<SpriteSourcePannel>();
		auto pannel1 = std::make_unique<FrameSelectionPannel>();
		auto pannel2 = std::make_unique<ClipPropertiesPannel>();
		auto pannel3 = std::make_unique<ClipPreviewPannel>();

		AddPannel(std::move(pannel));
		AddPannel(std::move(pannel1));
		AddPannel(std::move(pannel2));
		AddPannel(std::move(pannel3));

		SetPannelData(m_PannelData);
		SetPannelParent();


		NEventManager* eventManager = NEventManager::Instance();


		SUBSCRIBE_EVENT(KeyPressEvent, &AnimationClipEditor::OnKeyPressed, eventManager, EventPriority::Low);
	}

	void AnimationClipEditor::OnDetach()
	{
	}


	void AnimationClipEditor::OnUpdate(float dt)
	{
		//Pannel data here


		if (m_CODA)
		{
			ImGui::OpenPopup("Unsaved Changes");

			if (ImGui::BeginPopupModal("Unsaved Changes", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				const char* message = "You have unsaved changes. Are you sure you want to close?";
				ImVec2 textSize = ImGui::CalcTextSize(message);
				ImVec2 windowSize = ImGui::GetContentRegionAvail();
				ImGui::SetCursorPosX((windowSize.x - textSize.x) * 0.5f);
				ImGui::Text("%s", message);

				ImGui::Dummy(ImVec2(0.0f, 10.0f));

				float buttonWidth = 100.0f;
				float buttonSpacing = ImGui::GetStyle().ItemSpacing.x;
				float totalWidth = buttonWidth * 3.0f + buttonSpacing * 2.0f;
				ImGui::SetCursorPosX((windowSize.x - totalWidth) * 0.5f);

				if (ImGui::Button("Save & Close", ImVec2(buttonWidth, 0)))
				{
					SaveCurrentClipImpl();
					m_CODA = false;
					Hide();
				}
				ImGui::SameLine();

				if (ImGui::Button("Don't Save", ImVec2(buttonWidth, 0)))
				{
					m_CODA = false;
					m_HasUnsavedChanges = false;
					Hide();
				}
				ImGui::SameLine();

				if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
				{
					m_CODA = false;
				}

				ImGui::EndPopup();
			}

		}
		KeyboardShortcuts();
	}

	bool AnimationClipEditor::HandleKeyboardShortcuts(const KeyPressEvent& e)
	{
		if (NAsyncTaskManager::Instance()->HasPendingTasks())
			return false;


		return true;
	}

	void AnimationClipEditor::AddPannel(std::unique_ptr<AnimationPannel>&& pannel)
	{
		m_Pannels.push_back(std::move(pannel));
	}

	void AnimationClipEditor::SetPannelData(const AnimationPannelData& data)
	{
		for (auto& pannel : m_Pannels)
			pannel.get()->SetPannelData(m_PannelData);
	}
	void AnimationClipEditor::SetPannelParent()
	{
		for (auto& pannel : m_Pannels)
			pannel.get()->SetPannelParent(this);
	}

	void AnimationClipEditor::OnRender()
	{
		for (auto& panel : m_Pannels)
			panel->OnImGUIRender();
	}

	void AnimationClipEditor::RenderMenuBar()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Clip", "CTRL+N", false))
			{
				NewClipImpl();
			}
			if (ImGui::MenuItem("Open Clip Library", "CTRL+O", false))
			{
				OpenClipLibraryImpl();
			}
			if (ImGui::MenuItem("Save Clip", "CTRL+S", false))
			{
				SaveCurrentClipImpl();
			}
			if (ImGui::MenuItem("Export Clips", "CTRL+E", false))
			{
				ExportClipsImpl();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Close Editor", NULL, false))
			{
				if (m_HasUnsavedChanges)
				{
					m_CODA = true;
				}
				else
				{
					Hide();
				}
			}
			ImGui::EndMenu();
		}
	}
	void AnimationClipEditor::OnEvent(const Event& e)
	{
		
	}


	void AnimationClipEditor::KeyboardShortcuts()
	{
		if (NAsyncTaskManager::Instance()->HasPendingTasks())
			return;

		if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) && ImGui::IsKeyPressed(ImGuiKey_N))
		{
		}

		if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) && ImGui::IsKeyPressed(ImGuiKey_O))
		{
		}

		if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) &&
			(ImGui::IsKeyDown(ImGuiKey_LeftAlt) || ImGui::IsKeyDown(ImGuiKey_RightAlt)) && ImGui::IsKeyPressed(ImGuiKey_S))
		{
		}

		if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) &&
			(ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift)) && ImGui::IsKeyPressed(ImGuiKey_S))
		{
		}

		if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) && ImGui::IsKeyPressed(ImGuiKey_S))
		{
		}
	}


	bool AnimationClipEditor::OnKeyPressed(const KeyPressEvent& e)
	{
		HandleKeyboardShortcuts(e);
		return true;
	}

	//bool AnimationClipEditor::OnSceneSwitched(const SceneSwitchEvent& e)
	//{
	//	return true;
	//}

	void AnimationClipEditor::ExportClipsImpl()
	{ }
	void AnimationClipEditor::SaveCurrentClipImpl()
	{ }
	void AnimationClipEditor::OpenClipLibraryImpl()
	{ }
	void AnimationClipEditor::NewClipImpl()
	{ }
}
