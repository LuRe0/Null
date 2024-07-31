#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	ImGuiLayer.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Submodules/Layers/Layer.h"

#include "Null/Engine/Submodules/Graphics/Camera/Controllers/OrthographicCameraController.h"
#include "Null/Engine/Submodules/Graphics/Camera/Controllers/PerspectiveCameraController.h"

//#include "Pannels/SceneHierarchyPannel.h"


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//


namespace NULLENGINE
{

	struct PannelData
	{
		EntityID m_SelectedEntity = 0;
		Scene* m_Context = nullptr;
	};

	class NLE_API CameraController;
	class NLE_API OrthographicCameraController;
	class NLE_API PerspectiveCameraController;

	class NLE_API Pannel
	{
	public:
		Pannel() = default;
		virtual ~Pannel() {};

		void SetPannelData(PannelData& data) { m_PannelData = &data; }
		void SetPannelParent(ImGuiLayer* parent) { m_Parent = parent; }

		virtual void OnImGUIRender() = 0;
	protected:
		PannelData* m_PannelData;

		ImGuiLayer* m_Parent;
	};


	class NLE_API ImGuiLayer : public ILayer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() override;

		 void OnAttach() override;

		 void OnUpdate(float dt) override;

		 void OnRender() override;

		 void OnImGUIRender();

		 void OnDetach() override;

		 void OnEvent(const Event& event) override;

		 void AddPannel(std::unique_ptr<Pannel>&& overlay);

		 void SetPannelData(const PannelData& data);
		 void SetPannelParent();

		 void SetCamera(Camera::CameraType type);

		 void SetGuizmo(int g);

		 Camera* GetCurrentCamera();

		 CameraController* GetCameraController();

		 void Begin();
		 void End();

	private:
		ImGuiLayer(ImGuiLayer const&) = delete;
		ImGuiLayer& operator=(ImGuiLayer const&) = delete;

		std::vector<std::unique_ptr<Pannel>> m_Pannels;

		PannelData m_PannelData;

		CameraController* m_CameraController;

		std::unique_ptr<OrthographicCameraController> m_CameraController2D;
		std::unique_ptr<PerspectiveCameraController> m_CameraController3D;

		glm::vec2 m_ViewportSize = {0,0};
		glm::vec2 m_viewportBounds[2];

		int m_GuizmoType = -1;

		bool m_FlyMode;

		void ImGuizmoImpl();

		void InitCameraControllers();

		void KeyboardShortcuts(const KeyPressEvent& e);

		//std::unique_ptr<SceneHierarchyPannel> m_SceneHierachyPannel;

		// Member functions for handling events
		void OnWindowResize(const WindowResizeEvent& e);
		void OnWindowClose(const WindowCloseEvent& e);
		void OnKeyPressed(const KeyPressEvent& e);
		void OnKeyReleased(const KeyReleaseEvent& e);
		void OnKeyTyped(const KeyTypedEvent& e);
		//void OnKeyHold(const KeyHoldEvent& event);
		void OnMousePressed(const MouseButtonPressEvent& e);
		void OnMouseReleased(const MouseButtonReleaseEvent& e);
		void OnMouseMove(const MouseMoveEvent& e);
		void OnMouseScroll(const MouseScrolledEvent& e);
	};

}