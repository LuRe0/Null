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

#include "../Pannels/SceneHierarchyPannel.h"
#include "../Pannels/ComponentInspectorPannel.h"
#include "../Pannels/ScenePropertyPannel.h"
#include "../Pannels/ContentBrowserPannel.h"
#include "../Pannels/ModuleStatsPannel.h"
#include "../Pannels/EditorConsolePannel.h"
#include "../Pannels/EditorToolbarPannel.h"
#include "../Pannels/TimePannel.h"
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
		//auto pannel = std::make_unique<SceneHierarchyPannel>();
		//auto pannel2 = std::make_unique<ComponentInspectorPannel>();
		//auto pannel3 = std::make_unique<ScenePropertyPannel>();
		//auto pannel4 = std::make_unique<ContentBrowserPannel>();
		//auto pannel5 = std::make_unique<ModuleStatsPannel>();
		//auto pannel6 = std::make_unique<EditorConsolePannel>();
		//auto pannel7 = std::make_unique<EditorToolbarPannel>();
		//auto pannel8 = std::make_unique<TimePannel>();

		//AddPannel(std::move(pannel));
		//AddPannel(std::move(pannel2));
		//AddPannel(std::move(pannel3));
		//AddPannel(std::move(pannel4));
		//AddPannel(std::move(pannel5));
		//AddPannel(std::move(pannel6));
		//AddPannel(std::move(pannel7));
		//AddPannel(std::move(pannel8));

		//SetPannelData(m_PannelData);
		//SetPannelParent();


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
			ImGui::OpenPopup("CODA");


			if (ImGui::BeginPopupModal("CODA", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				// Center the text horizontally
				const char* message = "Are you sure you want to exit the NULL Editor?";
				ImVec2 textSize = ImGui::CalcTextSize(message);
				ImVec2 windowSize = ImGui::GetContentRegionAvail();
				ImGui::SetCursorPosX((windowSize.x - textSize.x) * 0.5f);
				ImGui::Text(" %s", message);

				// Add some vertical space before buttons
				ImGui::Dummy(ImVec2(0.0f, 10.0f));

				// Center the buttons horizontally
				float buttonWidth = 120.0f;
				float buttonSpacing = ImGui::GetStyle().ItemSpacing.x;
				float totalWidth = buttonWidth * 2.0f + buttonSpacing;
				ImGui::SetCursorPosX((windowSize.x - totalWidth) * 0.5f);

				// OK button
				if (ImGui::Button("OK", ImVec2(buttonWidth, 0)))
				{
					ImGui::CloseCurrentPopup();
					m_CODA = false;
					NWindow* window = NWindow::Instance();
					window->CloseWindow();
				}

				ImGui::SameLine();

				// Cancel button
				if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
				{
					m_CODA = false;
					ImGui::CloseCurrentPopup();
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
				Hide(); // Instead of m_CODA
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
}
