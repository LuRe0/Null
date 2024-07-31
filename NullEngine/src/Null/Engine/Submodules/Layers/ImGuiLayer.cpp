
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

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include <ImGuizmo/ImGuizmo.h>
#include <box2d/b2_body.h>
#include <magic_enum/magic_enum.hpp>


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

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

		NWindow* window = NEngine::Instance().Get<NWindow>();
		ImGui_ImplGlfw_InitForOpenGL(window->GetWinddow(), true);
		ImGui_ImplOpenGL3_Init("#version 410");

		SetPannelData(m_PannelData);
		SetPannelParent();


		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();


		SUBSCRIBE_EVENT(KeyPressEvent, &ImGuiLayer::OnKeyPressed, eventManager, EventPriority::Low);

		InitCameraControllers();
	}
	void ImGuiLayer::OnUpdate(float dt)
	{
		m_FlyMode = Input::MouseHold(GLFW_MOUSE_BUTTON_2);

		NSceneManager* scMan = NEngine::Instance().Get<NSceneManager>();

		m_PannelData.m_Context = scMan->GetCurrentScene();

		if (m_FlyMode)
			m_CameraController->Update(dt);
	}

	void ImGuiLayer::OnRender()
	{
		Begin();
		OnImGUIRender();
		End();
	}

	void ImGuiLayer::OnImGUIRender()
	{
		NRenderer* renderer = NEngine::Instance().Get<NRenderer>();
		NWindow* window = NEngine::Instance().Get<NWindow>();
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
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
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
				if (ImGui::MenuItem("New Scene", NULL, false))
				{
					NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

					eventManager->QueueEvent(std::make_unique<SceneSwitchEvent>(m_PannelData.m_Context->m_Name, "New Scene"));

					m_PannelData.m_SelectedEntity = {};
				}

				if (ImGui::MenuItem("Open", NULL, false))
				{

					const std::string& nextScene = FileDialog::OpenFile("Null Engine Scene (*.json)\0*.json\0");

					if (!nextScene.empty())
					{
						NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

						eventManager->QueueEvent(std::make_unique<SceneSwitchEvent>(m_PannelData.m_Context->m_Name, nextScene));

						m_PannelData.m_SelectedEntity = {};
					}
				}

				if (ImGui::MenuItem("Save Scene", NULL, false))
				{
					m_PannelData.m_Context->Serialize();
				}

				if (ImGui::MenuItem("Save Scene As", NULL, false))
				{
					const std::string& nextScene = FileDialog::SaveFile("Null Engine Scene (*.json)\0*.json\0");

					m_PannelData.m_Context->Serialize(nextScene);
				}

				if (ImGui::MenuItem("Set As Start Scene", NULL, false))
				{
					m_PannelData.m_Context->SetAsStartScene();
				}

				if (ImGui::MenuItem("Exit", NULL, false))
				{
					NWindow* window = NEngine::Instance().Get<NWindow>();
					window->CloseWindow();
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

		//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));  // Reduce padding inside the frame
		//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 1.0f));   // Reduce spacing between items

		for (auto& pannel : m_Pannels)
			pannel.get()->OnImGUIRender();

		//ImGui::PopStyleVar(2);

		ImGui::Begin("Scene");
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		const Framebuffer& buffer = renderer->GetFramebuffer("Scene");
		uint32_t texture = buffer.GetColorAttachment(0);

		window->SetBlockEvents(!ImGui::IsWindowHovered() && !ImGui::IsWindowFocused());

		m_CameraController->SetEnabled(!(!ImGui::IsWindowHovered() && !ImGui::IsWindowFocused()));

		auto viewportOffet = ImGui::GetCursorPos();

		if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
		{
			renderer->ResizeFramebuffer(viewportPanelSize.x, viewportPanelSize.y);
			m_CameraController->OnResize(viewportPanelSize.x, viewportPanelSize.y);
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


		int mouseX = static_cast<int>(mousePos.x);
		int mouseY = static_cast<int>(mousePos.y);


	
		if (mouseX >= 0 && mouseY > 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			buffer.Bind();
	
			auto pixel = buffer.ReadPixels(1, mouseX, mouseY);
			NLE_CORE_WARN("PixelData = {0}", pixel);
			buffer.Unbind();
			if (ImGui::IsItemClicked() && pixel > 0)
			{
				m_PannelData.m_SelectedEntity = pixel;
			}
		}


		ImVec2 buttonSize = { 32,32 };
		ImGui::SetCursorPos({ 9, 31 });

		// set background color
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 4));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));


		ImGui::BeginChild("TOOLS", ImVec2{ 48, 200 }, true);


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

		ImGui::End();
	}
	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(const Event& e)
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		eventManager->TriggerEvent(e);
	}

	void ImGuiLayer::AddPannel(std::unique_ptr<Pannel>&& pannel)
	{
		m_Pannels.push_back(std::move(pannel));
	}

	void ImGuiLayer::SetPannelData(const PannelData& data)
	{
		for (auto& pannel : m_Pannels)

			pannel.get()->SetPannelData(m_PannelData);
	}
	void ImGuiLayer::SetPannelParent()
	{
		for (auto& pannel : m_Pannels)
			pannel.get()->SetPannelParent(this);
	}

	void ImGuiLayer::SetCamera(Camera::CameraType type)
	{
		if (m_CameraController->GetCamera()->GetCameraType() == type)
			return;

		NCameraManager* cameraManager = NEngine::Instance().Get<NCameraManager>();

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
			NLE_CORE_ASSERT(false, "Invalid Camera Type", magic_enum::enum_name(type).data());
		}


		cameraManager->SetCurrentCamera(m_CameraController->GetCamera()->GetName());
	}

	void ImGuiLayer::SetGuizmo(int g)
	{
		m_GuizmoType = g;

		//m_FlyMode = !(g > 0);
	}

	CameraController* ImGuiLayer::GetCameraController()
	{
		return m_CameraController;
	}

	Camera* ImGuiLayer::GetCurrentCamera()
	{
		return m_CameraController->GetCamera();
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
		NWindow* window = NEngine::Instance().Get<NWindow>();

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(window->Width(), window->Height());

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



	void ImGuiLayer::ImGuizmoImpl()
	{
		if (m_GuizmoType == -1)
			return;

		if (m_PannelData.m_SelectedEntity)
		{
			Entity& entity = m_PannelData.m_Context->GetEntity(m_PannelData.m_SelectedEntity);
			NCameraManager* camManager = NEngine::Instance().Get<NCameraManager>();
			NWindow* window = NEngine::Instance().Get<NWindow>();
			Camera* mainCam = camManager->GetCurrentCamera();

			ImGuizmo::SetOrthographic(mainCam->GetCameraType() == Camera::ORTHOGRAPHIC);
			ImGuizmo::SetDrawlist();

			float winWidth = (float)ImGui::GetWindowWidth();
			float winHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, winWidth, winHeight);


			glm::mat4 view = mainCam->GetViewMatrix();
			const glm::mat4& projectionMatrix = mainCam->GetProjectionMatrix();

			if (entity.Has<TransformComponent>())
			{
				TransformComponent& transform = entity.Get<TransformComponent>();

				glm::mat4 transformMatrix = transform.m_TransformMatrix;

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
					DecomposeTransform(transformMatrix, translation, rotation, scale);

					glm::vec3 deltaRotation = glm::degrees(rotation) - transform.m_Rotation;


					transform.m_Translation = translation;
					transform.m_Rotation += deltaRotation;
					transform.m_Scale = scale;

					if (entity.Has<Rigidbody2DComponent>())
					{
						PhysicsSystem* physicsSys = NEngine::Instance().Get<PhysicsSystem>();

						Rigidbody2DComponent& rb2d = entity.Get<Rigidbody2DComponent>();

						auto pos = physicsSys->PixelsToMeters(transform.m_Translation.x, transform.m_Translation.y);

						if (rb2d.m_RuntimeBody)
							rb2d.m_RuntimeBody->SetTransform({ pos.x, pos.y }, transform.m_Rotation.z);
					}

					transform.m_Dirty = true;

				}
			}
		}
	}

	void ImGuiLayer::InitCameraControllers()
	{
		NCameraManager* cameraManager = NEngine::Instance().Get<NCameraManager>();

		m_CameraController2D = std::make_unique<OrthographicCameraController>();
		m_CameraController3D = std::make_unique<PerspectiveCameraController>();

		Camera* camera3D = cameraManager->GetCamera<Camera3D>("Editor3D");
		Camera* camera2D = cameraManager->GetCamera<Camera2D>("Editor2D");


		NLE_CORE_ASSERT(camera3D != nullptr || camera2D != nullptr, "Missing necessary Editor!");


		m_CameraController = m_CameraController2D.get();
		m_CameraController->SetCamera(camera2D);
		m_CameraController->SetEnabled(false);
		m_CameraController->Init();

		m_CameraController = m_CameraController3D.get();
		m_CameraController->SetCamera(camera3D);
		m_CameraController->Init();
	}

	void ImGuiLayer::KeyboardShortcuts(const KeyPressEvent& e)
	{
		if (m_FlyMode)
			return;

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
	}

	void ImGuiLayer::OnWindowResize(const WindowResizeEvent& e)
	{
		//ImGuiIO& io = ImGui::GetIO();

		//io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
		//io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		//glViewport(0, 0, e.GetWidth(), e.GetHeight());
	}

	void ImGuiLayer::OnWindowClose(const WindowCloseEvent& e)
	{
		NLE_CORE_INFO("{0}", e.Print());
	}

	void ImGuiLayer::OnKeyPressed(const KeyPressEvent& e)
	{
		KeyboardShortcuts(e);
	}

	void ImGuiLayer::OnKeyReleased(const KeyReleaseEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.KeysDown[e.GetKeyCode()] = false;
	}

	void ImGuiLayer::OnKeyTyped(const KeyTypedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		int keycode = e.GetKeyCode();

		if (keycode > 0 && keycode < 0x10000)
			io.AddInputCharacter((unsigned short)keycode);
	}

	//void ImGuiLayer::OnKeyHold(const KeyHoldEvent& event)
	//{
	//}

	void ImGuiLayer::OnMousePressed(const MouseButtonPressEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.MouseDown[e.GetButton()] = true;
	}


	void ImGuiLayer::OnMouseReleased(const MouseButtonReleaseEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.MouseDown[e.GetButton()] = false;
	}

	void ImGuiLayer::OnMouseMove(const MouseMoveEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.MousePos = ImVec2(e.GetX(), e.GetY());
	}

	void ImGuiLayer::OnMouseScroll(const MouseScrolledEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.MouseWheelH += e.GetXOffset();
		io.MouseWheel += e.GetYOffset();

	}

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
	{
		// From glm::decompose in matrix_decompose.inl

		using namespace glm;
		using T = float;

		mat4 LocalMatrix(transform);

		// Normalize the matrix.
		if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
			return false;

		// First, isolate perspective.  This is the messiest.
		if (
			epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
		{
			// Clear the perspective partition
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
			LocalMatrix[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		translation = vec3(LocalMatrix[3]);
		LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

		vec3 Row[3], Pdum3;

		// Now get scale and shear.
		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(Row[0]);
		Row[0] = detail::scale(Row[0], static_cast<T>(1));
		scale.y = length(Row[1]);
		Row[1] = detail::scale(Row[1], static_cast<T>(1));
		scale.z = length(Row[2]);
		Row[2] = detail::scale(Row[2], static_cast<T>(1));

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

		rotation.y = asin(-Row[0][2]);
		if (cos(rotation.y) != 0) {
			rotation.x = atan2(Row[1][2], Row[2][2]);
			rotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else {
			rotation.x = atan2(-Row[2][0], Row[1][1]);
			rotation.z = 0;
		}


		return true;


		return true;
	}


}