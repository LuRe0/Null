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
#include "AnimationGraphEditor.h"
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
	AnimationGraphEditor::AnimationGraphEditor() : ImGuiEditor("Scene", EditorType::SCENE), m_viewportBounds{ glm::vec2(0.0f), glm::vec2(0.0f) }
	{
		m_IsOpen = false; // Scene editor always open by default

		OnAttach();
	}

	void AnimationGraphEditor::OnAttach()
	{
		// handle pannels here

		NEventManager* eventManager = NEventManager::Instance();


		SUBSCRIBE_EVENT(KeyPressEvent, &AnimationGraphEditor::OnKeyPressed, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(SceneSwitchEvent, &AnimationGraphEditor::OnSceneSwitched, eventManager, EventPriority::High);

	}

	void AnimationGraphEditor::OnDetach()
	{
	}


	void AnimationGraphEditor::OnUpdate(float dt)
	{

	}

	bool AnimationGraphEditor::HandleKeyboardShortcuts(const KeyPressEvent& e)
	{
		//if (m_FlyMode || NAsyncTaskManager::Instance()->HasPendingTasks())
		//	return false;

		//switch (e.GetKeyCode())
		//{
		//case GLFW_KEY_Q:
		//	break;
		//case GLFW_KEY_W:
		//	break;
		//case GLFW_KEY_E:
		//	break;
		//case GLFW_KEY_R:
		//	break;
		//case GLFW_KEY_T:
		//	break;
		//}

		return true;
	}

	void AnimationGraphEditor::OnRender()
	{

		// If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
			// In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
			// In this specific demo, we are not using DockSpaceOverViewport() because:
			// - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
			// - we allow the host window to have padding (when opt_padding == true)
			// - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
			// TL;DR; this demo is more complicated than what you would normally use.
			// If we removed all the options we are showcasing, this demo would become:
			//     void ShowExampleAppDockSpace()
			//     {
			//         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
			//     }

		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("##DockSpace Demo", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "CTRL+N", false))
				{
				}

				if (ImGui::MenuItem("Open", "CTRL+O", false))
				{
				}

				if (ImGui::MenuItem("Save Scene", "CTRL+S", false))
				{
				}

				if (ImGui::MenuItem("Save Scene As", "CTRL+SHIFT+S", false))
				{
				}

				if (ImGui::MenuItem("Set As Start Scene", "CTRL+ALT+S", false))
				{
				}

				if (ImGui::MenuItem("Exit", NULL, false))
				{
					m_CODA = true;
				}

				ImGui::EndMenu();
			}



			if (ImGui::BeginMenu("Options"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
				ImGui::MenuItem("Padding", NULL, &opt_padding);
				ImGui::Separator();

				if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
				if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
				if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
				if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
				if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
				ImGui::Separator();

				if (ImGui::MenuItem("Close", NULL, false))
					dockspaceOpen = false;
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		if (m_CODA)
		{
			ImGui::OpenPopup("CODA");
		}

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

		//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));  // Reduce padding inside the frame
		//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 1.0f));   // Reduce spacing between items

		for (auto& pannel : m_Pannels)
			pannel.get()->OnImGUIRender();



		ImGui::End();
	}

	void AnimationGraphEditor::OnEvent(const Event& e)
	{
		
	}

	void AnimationGraphEditor::AddPannel(std::unique_ptr<AnimationPannel>&& pannel)
	{
		m_Pannels.push_back(std::move(pannel));
	}




	//glm::vec2 AnimationGraphEditor::GetMousePosition()
	//{
	//	auto windowSize = ImGui::GetWindowSize();
	//	ImVec2 windowPos = ImGui::GetWindowPos();

	//	// Get the content region position and size
	//	ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
	//	ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();

	//	// Calculate the actual bounds of the viewport within the window
	//	ImVec2 minBound = { windowPos.x + contentRegionMin.x, windowPos.y + contentRegionMin.y };
	//	ImVec2 maxBound = { windowPos.x + contentRegionMax.x, windowPos.y + contentRegionMax.y };
	//	glm::vec2 viewportBounds[2];
	//	viewportBounds[0] = { minBound.x, minBound.y };
	//	viewportBounds[1] = { maxBound.x, maxBound.y };

	//	glm::vec2 viewportSize = viewportBounds[1] - viewportBounds[0];

	//	auto mousePos = ImGui::GetMousePos();

	//	mousePos.x -= viewportBounds[0].x;
	//	mousePos.y -= viewportBounds[0].y;

	//	return glm::vec2(mousePos.x, mousePos.y);
	//}

	

	bool AnimationGraphEditor::OnKeyPressed(const KeyPressEvent& e)
	{
		HandleKeyboardShortcuts(e);
		return true;
	}

	bool AnimationGraphEditor::OnSceneSwitched(const SceneSwitchEvent& e)
	{
		m_PannelData.m_SelectedEntity = {};
		return true;
	}
}
