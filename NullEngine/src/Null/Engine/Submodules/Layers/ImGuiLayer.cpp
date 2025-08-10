
//------------------------------------------------------------------------------
//
// File Name:	ImGuiLayer.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "ImGuiLayer.h"
// temporary
//#include "imgui.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "Null/Tools/FileDialog.h"


#include <ImGuizmo/ImGuizmo.h>
#include <box2d/b2_body.h>
#include <magic_enum/magic_enum.hpp>

#include "../Scene.h"
#include "../../../../../../NullEditor/src/Editors/ImGuiEditor.h"
#include "../../../../../../NullEditor/src/Editors/SceneEditor.h"
#include "../../../../../../NullEditor/src/Editors/AnimationClipEditor.h"
#include "../../../../../../NullEditor/src/Editors/AnimationMontageEditor.h"
#include "NIncludes.h"
//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{

	ImGuiLayer::ImGuiLayer() 
	{

	}
	ImGuiLayer::~ImGuiLayer()
	{

	}
	void ImGuiLayer::OnAttach()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		io.FontGlobalScale = 1.25f;
		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		NWindow* window = NWindow::Instance();
		ImGui_ImplGlfw_InitForOpenGL(window->GetWinddow(), true);
		ImGui_ImplOpenGL3_Init("#version 410");


		AddEditor(std::make_unique<SceneEditor>());
		AddEditor(std::make_unique<AnimationClipEditor>());
		AddEditor(std::make_unique<AnimationMontageEditor>());
	}
	void ImGuiLayer::OnUpdate(float dt)
	{
		//KeyboardShortcuts();
		for (auto& editor : m_Editors)
		{
			if (editor->IsOpen())
			{
				editor->OnUpdate(dt);
			}
		}
	}

	void ImGuiLayer::OnRender()
	{
		Begin();
		OnImGUIRender();
		End();
	}

	void ImGuiLayer::AddEditor(std::unique_ptr<ImGuiEditor>&& editor)
	{
		m_Editors.push_back(std::move(editor));
	}

	void ImGuiLayer::RenderContextSensitiveMenuBar()
	{
		if(!m_CurrentEditor)
			return;

		ImGui::SetWindowFontScale(1.250f);
		m_CurrentEditor->RenderMenuBar();
		ImGui::SetWindowFontScale(1.0f);
	}

	//void ImGuiLayer::OnImGUIRender()
	//{
	//	for (auto& editor : m_Editors)
	//	{
	//		if (editor->IsOpen())
	//		{
	//			editor->OnRender();
	//		}
	//	}
	//}

	void ImGuiLayer::OnImGUIRender()
	{
		// Main menu bar
		if (ImGui::BeginMainMenuBar())
		{
			RenderContextSensitiveMenuBar();



			if (ImGui::BeginMenu("Options"))
			{
				ImGui::EndMenu();
			}



			if (ImGui::BeginMenu("Editors"))
			{
				for (auto& editor : m_Editors) 
				{
					bool isOpen = editor->IsOpen();
					if (ImGui::MenuItem(editor->GetName().c_str(), nullptr, &isOpen))
					{
						if (isOpen)
						{
							editor->Show();
							editor->SetShouldSelect(true);
						}
						else
							editor->Hide();
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		CreateMainDockspace();
	}

	void ImGuiLayer::CreateMainDockspace()
	{
		// Create fullscreen invisible window
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		if (ImGui::Begin("MainDockSpace", nullptr, window_flags))
		{
			ImGui::PopStyleVar(3);

			// Editor tabs
			if (ImGui::BeginTabBar("EditorTabs", ImGuiTabBarFlags_Reorderable))
			{
				for (auto& editor : m_Editors)
				{
					if (editor->IsOpen())
					{
						bool tabOpen = true;

						ImGuiTabItemFlags flags = ImGuiTabItemFlags_None;

						// Check if this tab should be selected
						if (editor->ShouldSelect())
						{
							flags |= ImGuiTabItemFlags_SetSelected;
							editor->SetShouldSelect(false); // Reset the flag
						}

						if (editor->HasUnsavedChanges())
							flags |= ImGuiTabItemFlags_UnsavedDocument;

						bool tabJustClosed = false;
						if (ImGui::BeginTabItem(editor->GetName().c_str(), &tabOpen, flags))
						{
							m_CurrentEditor = editor.get(); // Track active editor for context menu

							// Create unique dockspace for this editor
							ImGuiID dockspace_id = ImGui::GetID((editor->GetName() + "_DockSpace").c_str());
							ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

							// Render editor content
							editor->OnRender();

							ImGui::EndTabItem();
						}

						// If user closed tab, close editor
						if (!tabOpen)
							editor->Hide();

						if (!tabOpen && editor->HasUnsavedChanges())
						{
							// Ask confirmation
							if (ImGui::BeginPopupModal("Unsaved Changes?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
							{
								ImGui::Text("You have unsaved changes. Save before closing?");
								if (ImGui::Button("Yes"))
								{
									editor->SaveChanges();

									ImGui::CloseCurrentPopup();
								}
								ImGui::SameLine();
								if (ImGui::Button("No"))
								{
									// Close without saving
									ImGui::CloseCurrentPopup();
								}
								ImGui::SameLine();
								if (ImGui::Button("Cancel"))
								{
									// Cancel close → reopen tab
									tabOpen = true;
									ImGui::CloseCurrentPopup();
								}
								ImGui::EndPopup();
							}
							else
							{
								// Open modal this frame
								ImGui::OpenPopup("Unsaved Changes?");
							}
						}
					}
				}
					ImGui::EndTabBar();
			}
			ImGui::End();
		}
	}

	//void ImGuiLayer::OnImGUIRender()
	//{
	//	// Your existing dockspace setup...
	//	RenderDockSpace();

	//	if (ImGui::BeginMenuBar())
	//	{
	//		RenderMainMenuBar();
	//		ImGui::EndMenuBar();
	//	}

	//	// Render all editors - each handles its own docking window
	//	for (auto& editor : m_Editors)
	//	{
	//		editor->OnRender();
	//	}

	//	ImGui::End(); // End dockspace
	//}

	//void ImGuiLayer::RenderMainMenuBar()
	//{
	//	if (ImGui::BeginMenu("File"))
	//	{
	//		// Scene operations (delegate to scene editor)
	//		if (ImGui::MenuItem("New Scene", "CTRL+N"))
	//			m_SceneEditor->NewSceneImpl();
	//		// ... other scene menu items
	//		ImGui::EndMenu();
	//	}

	//	if (ImGui::BeginMenu("Editors"))
	//	{
	//		if (ImGui::MenuItem("Scene Editor", "F1", m_SceneEditor->IsOpen()))
	//			m_SceneEditor->Toggle();

	//		if (auto* clipEditor = GetEditor(EditorType::ANIMATION_CLIP))
	//		{
	//			if (ImGui::MenuItem("Animation Clip Editor", "F2", clipEditor->IsOpen()))
	//				clipEditor->Toggle();
	//		}

	//		if (auto* stateMachineEditor = GetEditor(EditorType::STATE_MACHINE))
	//		{
	//			if (ImGui::MenuItem("State Machine Editor", "F3", stateMachineEditor->IsOpen()))
	//				stateMachineEditor->Toggle();
	//		}

	//		ImGui::EndMenu();
	//	}
	//}

	//bool ImGuiLayer::HandleGlobalShortcuts(const KeyPressEvent& e)
	//{
	//	// Global editor shortcuts
	//	if (ImGui::IsKeyPressed(ImGuiKey_F1)) {
	//		ShowEditor(EditorType::SCENE);
	//		return true;
	//	}
	//	if (ImGui::IsKeyPressed(ImGuiKey_F2)) {
	//		ShowEditor(EditorType::ANIMATION_CLIP);
	//		return true;
	//	}
	//	if (ImGui::IsKeyPressed(ImGuiKey_F3)) {
	//		ShowEditor(EditorType::STATE_MACHINE);
	//		return true;
	//	}

	//	// Let editors handle their shortcuts
	//	for (auto& editor : m_Editors)
	//	{
	//		if (editor->IsOpen() && editor->HandleKeyboardShortcuts(e))
	//			return true;
	//	}

	//	return false;
	//}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		for (auto& editor : m_Editors)
		{
			editor->OnDetach();
		}
	}

	void ImGuiLayer::OnEvent(const Event& e)
	{
		NEventManager* eventManager = NEventManager::Instance();

		eventManager->TriggerEvent(e);

		for (auto& editor : m_Editors)
		{
			if (editor->IsOpen())
			{
				editor->OnEvent(e);
			}
		}
	}


	void ImGuiLayer::Begin()
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		NWindow* window = NWindow::Instance();

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(static_cast<float>(window->Width()), static_cast<float>(window->Height()));

		// Rendering
		ImGui::Render();

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = window->GetWinddow();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	}



}