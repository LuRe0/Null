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
#include "SceneEditor.h"
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
	SceneEditor::SceneEditor() : ImGuiEditor("Scene", EditorType::SCENE), m_viewportBounds{ glm::vec2(0.0f), glm::vec2(0.0f) }, m_FlyMode(false), m_CameraController(nullptr)
	{
		m_IsOpen = true; // Scene editor always open by default

		OnAttach();
	}

	void SceneEditor::OnAttach()
	{
		auto pannel = std::make_unique<SceneHierarchyPannel>();
		auto pannel2 = std::make_unique<ComponentInspectorPannel>();
		auto pannel3 = std::make_unique<ScenePropertyPannel>();
		auto pannel4 = std::make_unique<ContentBrowserPannel>();
		auto pannel5 = std::make_unique<ModuleStatsPannel>();
		auto pannel6 = std::make_unique<EditorConsolePannel>();
		auto pannel7 = std::make_unique<EditorToolbarPannel>();
		auto pannel8 = std::make_unique<TimePannel>();

		pannel4.get()->SetID(STRID(m_Name));

		AddPannel(std::move(pannel));
		AddPannel(std::move(pannel2));
		AddPannel(std::move(pannel3));
		AddGenericPannel(std::move(pannel4));
		AddPannel(std::move(pannel5));
		AddPannel(std::move(pannel6));
		AddPannel(std::move(pannel7));
		AddPannel(std::move(pannel8));

		SetPannelData(m_PannelData);
		SetPannelParent();



		NEventManager* eventManager = NEventManager::Instance();


		SUBSCRIBE_EVENT(KeyPressEvent, &SceneEditor::OnKeyPressed, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(SceneSwitchEvent, &SceneEditor::OnSceneSwitched, eventManager, EventPriority::High);

		InitCameraControllers();
	}

	void SceneEditor::OnDetach()
	{
	}


	void SceneEditor::OnUpdate(float dt)
	{
		m_FlyMode = Input::MouseHold(GLFW_MOUSE_BUTTON_2);

		NSceneManager* scMan = NSceneManager::Instance();

		m_PannelData.m_Context = scMan->GetCurrentScene();

		if (m_FlyMode)
			m_CameraController->Update(dt);


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

	bool SceneEditor::HandleKeyboardShortcuts(const KeyPressEvent& e)
	{
		if (m_FlyMode || NAsyncTaskManager::Instance()->HasPendingTasks())
			return false;

		switch (e.GetKeyCode())
		{
		case GLFW_KEY_Q:
			SetGuizmo(-1);
			break;
		case GLFW_KEY_W:
			SetGuizmo(ImGuizmo::OPERATION::TRANSLATE);
			break;
		case GLFW_KEY_E:
			SetGuizmo(ImGuizmo::OPERATION::ROTATE);
			break;
		case GLFW_KEY_R:
			SetGuizmo(ImGuizmo::OPERATION::SCALE);
			break;
		case GLFW_KEY_T:
			SetGuizmo(ImGuizmo::OPERATION::UNIVERSAL);
			break;
		}

		return true;
	}

	//void SceneEditor::OnRender()
	//{

	//	// If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
	//		// In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
	//		// In this specific demo, we are not using DockSpaceOverViewport() because:
	//		// - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
	//		// - we allow the host window to have padding (when opt_padding == true)
	//		// - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
	//		// TL;DR; this demo is more complicated than what you would normally use.
	//		// If we removed all the options we are showcasing, this demo would become:
	//		//     void ShowExampleAppDockSpace()
	//		//     {
	//		//         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	//		//     }

	//	static bool dockspaceOpen = true;
	//	static bool opt_fullscreen = true;
	//	static bool opt_padding = false;
	//	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	//	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	//	// because it would be confusing to have two docking targets within each others.
	//	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	//	if (opt_fullscreen)
	//	{
	//		const ImGuiViewport* viewport = ImGui::GetMainViewport();
	//		ImGui::SetNextWindowPos(viewport->WorkPos);
	//		ImGui::SetNextWindowSize(viewport->WorkSize);
	//		ImGui::SetNextWindowViewport(viewport->ID);
	//		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	//		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	//		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	//		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	//	}
	//	else
	//	{
	//		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	//	}

	//	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	//	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	//	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
	//		window_flags |= ImGuiWindowFlags_NoBackground;

	//	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	//	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	//	// all active windows docked into it will lose their parent and become undocked.
	//	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	//	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	//	if (!opt_padding)
	//		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	//	ImGui::Begin("##DockSpace Demo", &dockspaceOpen, window_flags);
	//	if (!opt_padding)
	//		ImGui::PopStyleVar();

	//	if (opt_fullscreen)
	//		ImGui::PopStyleVar(2);

	//	// Submit the DockSpace
	//	ImGuiIO& io = ImGui::GetIO();
	//	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	//	{
	//		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	//		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	//	}

	//

	//	//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));  // Reduce padding inside the frame
	//	//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 1.0f));   // Reduce spacing between items

	//	for (auto& pannel : m_Pannels)
	//		pannel.get()->OnImGUIRender();





	//	ImGui::End();
	//}


	void SceneEditor::OnRender()
	{
		ImGuiEditor::OnRender();

		for (auto& panel : m_Pannels)
			panel->OnImGUIRender();

		switch (NEngine::Instance().GetEngineState())
		{
		case NULLENGINE::IEngine::EDIT:
		{
			WindowedEditSceneLayer();
			break;
		}
		case NULLENGINE::IEngine::PAUSE:
			break;
		case NULLENGINE::IEngine::RUN_MAXIMIZED: // Handle maximized state
			MaximizedSceneLayer();
			break;
		case NULLENGINE::IEngine::RUN_WINDOWED:  // Handle windowed state
			WindowedSceneLayer();
			break;
		case NULLENGINE::IEngine::SIMULATE:
			break;
		default:
			break;
		}
	}

	void SceneEditor::RenderMenuBar()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene", "CTRL+N", false))
			{
				NewSceneImpl();
			}

			if (ImGui::MenuItem("Open", "CTRL+O", false))
			{
				OpenSceneImpl();
			}

			if (ImGui::MenuItem("Save Scene", "CTRL+S", false))
			{
				SaveSceneImpl();
			}

			if (ImGui::MenuItem("Save Scene As", "CTRL+SHIFT+S", false))
			{
				SaveSceneAsImpl();
			}

			if (ImGui::MenuItem("Set As Start Scene", "CTRL+ALT+S", false))
			{
				SetAsDefaultSceneImpl();
			}

			if (ImGui::MenuItem("Exit", NULL, false))
			{
				m_CODA = true;
			}

			ImGui::EndMenu();
		}
	}

	void SceneEditor::OnEvent(const Event& e)
	{
		
	}

	void SceneEditor::AddPannel(std::unique_ptr<ScenePannel>&& pannel)
	{
		m_Pannels.push_back(std::move(pannel));
	}

	void SceneEditor::SaveChanges()
	{
		SaveSceneImpl();
		m_HasUnsavedChanges = false;
		NLE_CORE_INFO("Changes saved in Scene Editor");
	}

	void SceneEditor::SetPannelData(const ScenePannelData& data)
	{
		for (auto& pannel : m_Pannels)
			pannel.get()->SetPannelData(m_PannelData);
	}
	void SceneEditor::SetPannelParent()
	{
		for (auto& pannel : m_Pannels)
			pannel.get()->SetPannelParent(this);
	}



	glm::vec2 SceneEditor::GetMousePosition()
	{
		auto windowSize = ImGui::GetWindowSize();
		ImVec2 windowPos = ImGui::GetWindowPos();

		// Get the content region position and size
		ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
		ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();

		// Calculate the actual bounds of the viewport within the window
		ImVec2 minBound = { windowPos.x + contentRegionMin.x, windowPos.y + contentRegionMin.y };
		ImVec2 maxBound = { windowPos.x + contentRegionMax.x, windowPos.y + contentRegionMax.y };
		glm::vec2 viewportBounds[2];
		viewportBounds[0] = { minBound.x, minBound.y };
		viewportBounds[1] = { maxBound.x, maxBound.y };

		glm::vec2 viewportSize = viewportBounds[1] - viewportBounds[0];

		auto mousePos = ImGui::GetMousePos();

		mousePos.x -= viewportBounds[0].x;
		mousePos.y -= viewportBounds[0].y;

		return glm::vec2(mousePos.x, mousePos.y);
	}

	Camera* SceneEditor::GetCurrentCamera()
	{
		return m_CameraController->GetCamera();
	}


	void SceneEditor::SetCamera(int type)
	{
		if (m_CameraController->GetCamera()->GetCameraType() == type)
			return;

		NCameraManager* cameraManager = NCameraManager::Instance();

		if (type == Camera::ORTHOGRAPHIC)
		{
			m_CameraController->SetEnabled(false);
			m_CameraController = m_CameraController2D.get();
			m_CameraController->SetEnabled(true);
		}
		else if (type == Camera::PERSPECTIVE)
		{
			m_CameraController->SetEnabled(false);
			m_CameraController = m_CameraController3D.get();
			m_CameraController->SetEnabled(true);
		}
		else
		{
			NLE_CORE_ASSERT(false, "Invalid Camera Type", magic_enum::enum_name(static_cast<Camera::CameraType>(type)).data());
		}


		cameraManager->SetCurrentCamera(m_CameraController->GetCamera()->GetName());
	}

	void SceneEditor::SetRenderTarget(const std::string& fbName)
	{
		m_DisplayedFB = fbName;
	}

	void SceneEditor::SetGuizmo(int g)
	{
		m_GuizmoType = g;

		//m_FlyMode = !(g > 0);
	}


	void SceneEditor::InitCameraControllers()
	{
		NCameraManager* cameraManager = NCameraManager::Instance();

		m_CameraController2D = std::make_unique<OrthographicCameraController>();
		m_CameraController3D = std::make_unique<PerspectiveCameraController>();

		Camera* camera3D = cameraManager->GetCamera<Camera3D>(STRID("Editor3D"));
		Camera* camera2D = cameraManager->GetCamera<Camera2D>(STRID("Editor2D"));


		NLE_CORE_ASSERT(camera3D != nullptr || camera2D != nullptr, "Missing necessary Editor!");


		m_CameraController = m_CameraController2D.get();
		m_CameraController->SetCamera(camera2D);
		m_CameraController->SetEnabled(false);
		m_CameraController->Init();

		m_CameraController = m_CameraController3D.get();
		m_CameraController->SetCamera(camera3D);
		m_CameraController->Init();
	}



	void SceneEditor::WindowedEditSceneLayer()
	{
		NRenderer* renderer = NRenderer::Instance();
		NWindow* window = NWindow::Instance();
		NFramebufferManager* fbMan = NFramebufferManager::Instance();

		//ImGui::PopStyleVar(2);
		bool pOpen = true;
		ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoNav);
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		Framebuffer* buffer = fbMan->Get(m_DisplayedFB);
		Framebuffer* dataBuffer = fbMan->Get("Scene");
		uint32_t texture = buffer->GetColorAttachment(0);

		window->SetBlockEvents(!ImGui::IsWindowHovered() && !ImGui::IsWindowFocused() && !NAsyncTaskManager::Instance()->HasPendingTasks());

		m_CameraController->SetEnabled(!(!ImGui::IsWindowHovered() || !ImGui::IsWindowFocused()));

		auto viewportOffet = ImGui::GetCursorPos();

		if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
		{
			dataBuffer->Resize(static_cast<unsigned int>(viewportPanelSize.x), static_cast<unsigned int>(viewportPanelSize.y));
			buffer->Resize(static_cast<unsigned int>(viewportPanelSize.x), static_cast<unsigned int>(viewportPanelSize.y));
			//m_CameraController->OnResize(static_cast<unsigned int>(viewportPanelSize.x), static_cast<unsigned int>(viewportPanelSize.y));
			ResizeCamera();
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		}

		ImGui::Image((void*)(intptr_t)texture, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, { 0, 1 }, { 1, 0 });


		auto windowSize = ImGui::GetWindowSize();
		ImVec2 windowPos = ImGui::GetWindowPos();

		// Get the content region position and size
		ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
		ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();

		// Calculate the actual bounds of the viewport within the window
		ImVec2 minBound = { windowPos.x + contentRegionMin.x, windowPos.y + contentRegionMin.y };
		ImVec2 maxBound = { windowPos.x + contentRegionMax.x, windowPos.y + contentRegionMax.y };

		m_viewportBounds[0] = { minBound.x, minBound.y };
		m_viewportBounds[1] = { maxBound.x, maxBound.y };

		glm::vec2 viewportSize = m_viewportBounds[1] - m_viewportBounds[0];

		auto mousePos = ImGui::GetMousePos();

		mousePos.x -= m_viewportBounds[0].x;
		mousePos.y -= m_viewportBounds[0].y;

		Input::SetEditorMousePosition(mousePos.x, mousePos.y);

		int mouseX = static_cast<int>(mousePos.x);
		int mouseY = static_cast<int>(mousePos.y);



		if (mouseX >= 0 && mouseY >= 0 && mouseX <= (int)viewportSize.x && mouseY <= (int)viewportSize.y)
		{

			// Convert mouse coordinates to match OpenGL's bottom-left origin
			mouseY = static_cast<int>(viewportSize.y - mouseY);

			dataBuffer->Bind();

			auto pixel = dataBuffer->ReadPixels(1, mouseX, mouseY);

			//NLE_CORE_WARN("PixelData = {0}", pixel);

			//NLE_CORE_WARN("Mouse = {0},{1}", mouseX, mouseY); 

			dataBuffer->Unbind();
			if (ImGui::IsItemClicked() && !ImGuizmo::IsOver() && pixel > 0)
			{
				m_PannelData.m_SelectedEntity = pixel;

				if (m_GuizmoType == -1)
					SetGuizmo(ImGuizmo::OPERATION::TRANSLATE);
			}
			//else if (ImGui::IsMouseClicked(0) && !ImGuizmo::IsOver() && pixel <= 0)
			//{
			//	m_PannelData.m_SelectedEntity = {};
			//}
		}


		ImVec2 buttonSize = { 32,32 };
		ImGui::SetCursorPos({ 9, 31 });

		// set background color
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 4));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));


		ImGui::BeginChild("TOOLS", ImVec2{ 48, 200 }, true, ImGuiWindowFlags_NoNav);


		ImGui::BeginGroup();

		ImVec4 normalColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
		ImVec4 selectedColor = ImVec4(0.26f, 0.59f, 0.98f, 1.0f); // Blue color


		ImGui::PushStyleColor(ImGuiCol_Button, m_GuizmoType == -1 || m_FlyMode ? selectedColor : normalColor);
		if (ImGui::Button(m_FlyMode ? "F" : "V", buttonSize))
		{
			SetGuizmo(-1);
		}
		ImGui::PopStyleColor();


		// Button for Translate
		ImGui::PushStyleColor(ImGuiCol_Button, m_GuizmoType == ImGuizmo::OPERATION::TRANSLATE && !m_FlyMode ? selectedColor : normalColor);
		if (ImGui::Button("M", buttonSize))
		{
			SetGuizmo(ImGuizmo::OPERATION::TRANSLATE);
		}
		ImGui::PopStyleColor();
		// Button for Rotate
		ImGui::PushStyleColor(ImGuiCol_Button, m_GuizmoType == ImGuizmo::OPERATION::ROTATE && !m_FlyMode ? selectedColor : normalColor);
		if (ImGui::Button("R", buttonSize))
		{
			SetGuizmo(ImGuizmo::OPERATION::ROTATE);

		}
		ImGui::PopStyleColor();

		// Button for Scale
		ImGui::PushStyleColor(ImGuiCol_Button, m_GuizmoType == ImGuizmo::OPERATION::SCALE && !m_FlyMode ? selectedColor : normalColor);
		if (ImGui::Button("S", buttonSize))
		{
			SetGuizmo(ImGuizmo::OPERATION::SCALE);
		}
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_Button, m_GuizmoType == ImGuizmo::OPERATION::UNIVERSAL && !m_FlyMode ? selectedColor : normalColor);
		if (ImGui::Button("T", buttonSize))
		{
			SetGuizmo(ImGuizmo::OPERATION::UNIVERSAL);

		}
		ImGui::PopStyleColor();


		ImGui::EndGroup();
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(3);





		/// IMGUIZMO
		ImGuizmoImpl();



		ImGui::End();
	}

	void SceneEditor::WindowedSceneLayer()
	{
		NRenderer* renderer = NRenderer::Instance();
		NWindow* window = NWindow::Instance();
		NEventManager* eventManager = NEventManager::Instance();
		NCameraManager* camManager = NCameraManager::Instance();
		NFramebufferManager* fbMan = NFramebufferManager::Instance();

		//ImGui::PopStyleVar(2);
		bool pOpen = true;
		ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoNav);
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		Framebuffer* buffer = fbMan->Get("FinalOutput");
		uint32_t texture = buffer->GetColorAttachment(0);

		window->SetBlockEvents(!ImGui::IsWindowHovered() && !ImGui::IsWindowFocused());

		m_CameraController->SetEnabled(false);

		auto viewportOffet = ImGui::GetCursorPos();

		ImGui::Image((void*)(intptr_t)texture, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, { 0, 1 }, { 1, 0 });

		if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
		{
			buffer->Resize(static_cast<unsigned int>(viewportPanelSize.x), static_cast<unsigned int>(viewportPanelSize.y));
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
			ResizeCamera();
		}

		auto windowSize = ImGui::GetWindowSize();
		ImVec2 windowPos = ImGui::GetWindowPos();

		// Get the content region position and size
		ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
		ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();

		// Calculate the actual bounds of the viewport within the window
		ImVec2 minBound = { windowPos.x + contentRegionMin.x, windowPos.y + contentRegionMin.y };
		ImVec2 maxBound = { windowPos.x + contentRegionMax.x, windowPos.y + contentRegionMax.y };

		m_viewportBounds[0] = { minBound.x, minBound.y };
		m_viewportBounds[1] = { maxBound.x, maxBound.y };

		glm::vec2 viewportSize = m_viewportBounds[1] - m_viewportBounds[0];

		auto mousePos = ImGui::GetMousePos();

		mousePos.x -= m_viewportBounds[0].x;
		mousePos.y -= m_viewportBounds[0].y;


		int mouseX = static_cast<int>(mousePos.x);
		int mouseY = static_cast<int>(mousePos.y);
		Input::SetEditorMousePosition(mousePos.x, mousePos.y);

		ImGui::End();

		if (Input::KeyDown(GLFW_KEY_ESCAPE))
		{
			NEngine::Instance().SetEngineState(IEngine::EDIT);
			eventManager->QueueEvent(std::make_unique<SceneSwitchEvent>(m_PannelData.m_Context->GetName(), m_PannelData.m_Context->GetName()));
			eventManager->QueueEvent(std::make_unique<EngineEditStateEvent>(NEngine::EDIT));
		}
	}
	void SceneEditor::MaximizedSceneLayer()
	{
		NFramebufferManager* fbMan = NFramebufferManager::Instance();

		NRenderer* renderer = NRenderer::Instance();
		NWindow* window = NWindow::Instance();
		NEventManager* eventManager = NEventManager::Instance();


		// Get the size of the main viewport
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 viewportPos = viewport->Pos;
		ImVec2 viewSize = viewport->Size;

		// Set window flags for fullscreen
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoBringToFrontOnFocus;

		// Set the position and size to cover the entire viewport
		ImGui::SetNextWindowPos(viewportPos);
		ImGui::SetNextWindowSize(viewSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		// Begin the window
		//ImGui::Begin("Fullscreen Viewport", nullptr, windowFlags);

		//ImGui::PopStyleVar(2);
		//bool pOpen = true;
		ImGui::Begin("##Fullscreen Scene", nullptr, ImGuiWindowFlags_NoNavInputs);
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		Framebuffer* buffer = fbMan->Get("FinalOutput");
		uint32_t texture = buffer->GetColorAttachment(0);

		window->SetBlockEvents(!ImGui::IsWindowHovered() && !ImGui::IsWindowFocused());

		m_CameraController->SetEnabled(false);

		auto viewportOffet = ImGui::GetCursorPos();

		if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
		{
			buffer->Resize(static_cast<unsigned int>(viewportPanelSize.x), static_cast<unsigned int>(viewportPanelSize.y));
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
			ResizeCamera();
		}

		ImGui::Image((void*)(intptr_t)texture, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, { 0, 1 }, { 1, 0 });


		auto windowSize = ImGui::GetWindowSize();
		ImVec2 windowPos = ImGui::GetWindowPos();

		// Get the content region position and size
		ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
		ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();

		// Calculate the actual bounds of the viewport within the window
		ImVec2 minBound = { windowPos.x + contentRegionMin.x, windowPos.y + contentRegionMin.y };
		ImVec2 maxBound = { windowPos.x + contentRegionMax.x, windowPos.y + contentRegionMax.y };

		m_viewportBounds[0] = { minBound.x, minBound.y };
		m_viewportBounds[1] = { maxBound.x, maxBound.y };

		glm::vec2 viewportSize = m_viewportBounds[1] - m_viewportBounds[0];

		auto mousePos = ImGui::GetMousePos();

		mousePos.x -= m_viewportBounds[0].x;
		mousePos.y -= m_viewportBounds[0].y;


		int mouseX = static_cast<int>(mousePos.x);
		int mouseY = static_cast<int>(mousePos.y);

		Input::SetEditorMousePosition(mousePos.x, mousePos.y);

		ImGui::End();

		if (Input::KeyDown(GLFW_KEY_ESCAPE))
		{
			NEngine::Instance().SetEngineState(IEngine::EDIT);
			eventManager->QueueEvent(std::make_unique<SceneSwitchEvent>(m_PannelData.m_Context->GetName(), m_PannelData.m_Context->GetName()));
			eventManager->QueueEvent(std::make_unique<EngineEditStateEvent>(NEngine::EDIT));
		}
	}

	void SceneEditor::ResizeCamera()
	{
		NEventManager* eventManager = NEventManager::Instance();
		eventManager->QueueEvent(std::make_unique<WindowResizeEvent>(m_ViewportSize.x, m_ViewportSize.y));

		m_CameraController->OnResize(static_cast<unsigned int>(m_ViewportSize.x), static_cast<unsigned int>(m_ViewportSize.y));

	}


	void SceneEditor::ImGuizmoImpl()
	{
		if (m_GuizmoType == -1)
			return;

		if (m_PannelData.m_SelectedEntity && m_PannelData.m_Context->HasEntity(m_PannelData.m_SelectedEntity))
		{
			Entity& entity = m_PannelData.m_Context->GetEntity(m_PannelData.m_SelectedEntity);
			NCameraManager* camManager = NCameraManager::Instance();
			NWindow* window = NWindow::Instance();
			Camera* mainCam = camManager->GetCurrentCamera();

			ImGuizmo::SetOrthographic(mainCam->GetCameraType() == Camera::ORTHOGRAPHIC);
			ImGuizmo::SetDrawlist();

			float winWidth = m_ViewportSize.x;
			float winHeight = m_ViewportSize.y;
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, winWidth, winHeight);


			glm::mat4 view = mainCam->GetViewMatrix();
			const glm::mat4& projectionMatrix = mainCam->GetProjectionMatrix();

			if (entity.Has<TransformComponent>())
			{
				TransformComponent& transform = entity.Get<TransformComponent>();

				glm::mat4 transformMatrix = transform.transformMatrix;

				bool snap = Input::KeyDown(GLFW_KEY_LEFT_CONTROL);
				float snapValue = 0.5f;
				/*		if (m_GuizmoType == ImGuizmo::OPERATION::ROTATE)
							snapValue = 45.0f;*/

				float snapValues[3] = { snapValue, snapValue, snapValue };

				ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projectionMatrix), (ImGuizmo::OPERATION(m_GuizmoType)),
					ImGuizmo::LOCAL, glm::value_ptr(transformMatrix), nullptr, snap ? snapValues : nullptr);



				if (ImGuizmo::IsUsing())
				{

					glm::vec3 translation, rotation, scale;
					NULLENGINE::GLMHelper::DecomposeTransform(transformMatrix, translation, rotation, scale);

					const float epsilon = 1e-5f;  // Define a small tolerance value


					if ((m_GuizmoType & ImGuizmo::TRANSLATE) != 0)
					{

						if (glm::any(glm::epsilonNotEqual(transform.translation, translation, epsilon)))
						{
							if (entity.Has<ParentComponent>())
							{
								auto& parentComp = entity.Get<ParentComponent>();
								// Get the parent's world transform and update the child
								TransformComponent& parentTransform = entity.GetFromEntity<TransformComponent>(parentComp.m_Parent);

								glm::mat4 inverseParentTransform = glm::inverse(parentTransform.transformMatrix);

								auto localTranslation = glm::vec3((inverseParentTransform * glm::vec4(translation, 1.0f)));

								if (glm::any(glm::epsilonNotEqual(transform.translation, localTranslation, epsilon)))
								{
									transform.translation = localTranslation;
								}
							}
							else
							{
								transform.translation = translation;
							}

							transform.flags.Set(TransformFlags_DirectManipulation);
						}
					}

					if ((m_GuizmoType & ImGuizmo::ROTATE) != 0)
					{
						if (glm::any(glm::epsilonNotEqual(transform.rotation, rotation, epsilon)))
						{

							if (entity.Has<ParentComponent>())
							{
								auto& parentComp = entity.Get<ParentComponent>();
								// Get the parent's world transform and update the child
								TransformComponent& parentTransform = entity.GetFromEntity<TransformComponent>(parentComp.m_Parent);
								// Extract parent rotation as Euler angles (assume in degrees)
								glm::vec3 parentRotationEuler = parentTransform.rotation; // Euler angles in degrees

								// Convert parent rotation to quaternion
								glm::vec3 parentRotationRadians = glm::radians(parentRotationEuler);
								glm::quat parentRotation = glm::quat(glm::yawPitchRoll(parentRotationRadians.y, parentRotationRadians.x, parentRotationRadians.z));

								// Convert child rotation from Euler angles (assume in degrees)
								glm::vec3 childRotationEuler = glm::degrees(rotation); // Euler angles in degrees
								glm::vec3 childRotationRadians = glm::radians(childRotationEuler);
								glm::quat childRotation = glm::quat(glm::yawPitchRoll(childRotationRadians.y, childRotationRadians.x, childRotationRadians.z));

								// Compute local rotation by applying the inverse of the parent’s rotation
								glm::quat localRotation = glm::normalize(glm::inverse(parentRotation) * childRotation);

								// Convert local rotation back to Euler angles
								glm::vec3 localRotationEuler = glm::degrees(glm::eulerAngles(localRotation));

								glm::vec3 localdeltaRotation = localRotationEuler - transform.rotation;

								if (glm::length(localdeltaRotation) > epsilon)
								{
									transform.rotation += localdeltaRotation;
								}
							}
							else
							{
								glm::vec3 deltaRotation = glm::degrees(rotation) - transform.rotation;

								if (glm::length(deltaRotation) > epsilon)
								{
									transform.rotation += deltaRotation;
								}
							}
						}

						transform.flags.Set(TransformFlags_DirectManipulation);
					}

					if ((m_GuizmoType & ImGuizmo::SCALE) != 0)
					{

						if (glm::any(glm::epsilonNotEqual(transform.scale, scale, epsilon)))
						{
							if (entity.Has<ParentComponent>())
							{
								auto& parentComp = entity.Get<ParentComponent>();
								// Get the parent's world transform and update the child
								TransformComponent& parentTransform = entity.GetFromEntity<TransformComponent>(parentComp.m_Parent);


								auto localScale = scale / parentTransform.scale;

								if (glm::any(glm::epsilonNotEqual(transform.scale, localScale, epsilon)))
								{
									transform.scale = localScale;
								}
							}
							else
							{
								transform.scale = scale;
							}
						}
					}

					transform.flags.Set(TransformFlags_Dirty);

				}
			}
		}
	}

	void SceneEditor::NewSceneImpl()
	{
		NEngine::Instance().SetEngineState(IEngine::EDIT);

		if (NEngine::Instance().GetEngineState() != IEngine::EDIT)
		{
			NLE_CORE_WARN("Unable to create new scene during Game Runtime");
			return;
		}

		NEventManager* eventManager = NEventManager::Instance();

		eventManager->QueueEvent(std::make_unique<SceneSwitchEvent>(m_PannelData.m_Context->GetName(), "New Scene"));

		m_PannelData.m_SelectedEntity = {};
		NLE_CORE_INFO("New Scene Created");
	}

	void SceneEditor::SaveSceneImpl()
	{
		if (NEngine::Instance().GetEngineState() != IEngine::EDIT)
		{
			NLE_CORE_WARN("Unable to save during Game Runtime");
			return;
		}

		if (m_PannelData.m_Context->GetName() == "New Scene")
		{
			NLE_CORE_ERROR("Cannot save scene if name is: {0}", m_PannelData.m_Context->GetName());
		}
		else
		{
			m_PannelData.m_Context->Serialize();

			NLE_CORE_INFO("Scene: {0} successfully saved", m_PannelData.m_Context->GetName());
		}
	}

	void SceneEditor::SaveSceneAsImpl()
	{
		if (NEngine::Instance().GetEngineState() != IEngine::EDIT)
		{
			NLE_CORE_WARN("Unable to save during Game Runtime");
			return;
		}

		const std::string& nextScene = FileDialog::SaveFile("Null Engine Scene (*.json)\0*.json\0");

		m_PannelData.m_Context->Serialize(nextScene);

		NLE_CORE_INFO("Scene: {0} successfully saved as {1}", m_PannelData.m_Context->GetName(), nextScene);
	}
	void SceneEditor::OpenSceneImpl()
	{
		if (NEngine::Instance().GetEngineState() != IEngine::EDIT)
		{
			NLE_CORE_WARN("Unable to switch scene during Game Runtime");
			return;
		}

		const std::string& nextScene = FileDialog::OpenFile("Null Engine Scene (*.scene)\0*.scene\0");

		if (!nextScene.empty())
		{
			NEventManager* eventManager = NEventManager::Instance();

			eventManager->QueueEvent(std::make_unique<SceneSwitchEvent>(m_PannelData.m_Context->GetName(), nextScene));

			m_PannelData.m_SelectedEntity = {};

			NLE_CORE_INFO("Scene: {0} Loaded", nextScene);

		}
	}

	void SceneEditor::SetAsDefaultSceneImpl()
	{
		m_PannelData.m_Context->SetAsStartScene();

		NLE_CORE_INFO("Scene: {0} successfully set as default scene", m_PannelData.m_Context->GetName());
	}

	void SceneEditor::KeyboardShortcuts()
	{
		if (NAsyncTaskManager::Instance()->HasPendingTasks())
			return;

		if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) && ImGui::IsKeyPressed(ImGuiKey_N))
		{
			NewSceneImpl();
		}

		if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) && ImGui::IsKeyPressed(ImGuiKey_O))
		{
			OpenSceneImpl();
		}

		if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) &&
			(ImGui::IsKeyDown(ImGuiKey_LeftAlt) || ImGui::IsKeyDown(ImGuiKey_RightAlt)) && ImGui::IsKeyPressed(ImGuiKey_S))
		{
			SetAsDefaultSceneImpl();
		}

		if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) &&
			(ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift)) && ImGui::IsKeyPressed(ImGuiKey_S))
		{
			SaveSceneAsImpl();
		}

		if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) && ImGui::IsKeyPressed(ImGuiKey_S))
		{
			SaveSceneImpl();
		}
	}

	void SceneEditor::ResetViewportSize()
	{
		m_ViewportSize = {};
	}

	bool SceneEditor::OnKeyPressed(const KeyPressEvent& e)
	{
		HandleKeyboardShortcuts(e);
		return true;
	}

	bool SceneEditor::OnSceneSwitched(const SceneSwitchEvent& e)
	{
		m_PannelData.m_SelectedEntity = {};
		return true;
	}
}
