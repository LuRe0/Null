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
#include <glm/glm.hpp>

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
	class Sceene;
	using EntityID = std::uint32_t;

	struct PannelData
	{
		EntityID m_SelectedEntity = 0;
		Scene* m_Context = nullptr;
	};

	class NLE_API CameraController;
	class NLE_API OrthographicCameraController;
	class NLE_API PerspectiveCameraController;
	class NLE_API ImGuiLayer;

	class NLE_API Pannel
	{
	public:
		Pannel() = default;
		virtual ~Pannel() {};

		void SetPannelData(PannelData& data) { m_PannelData = &data; }
		void SetPannelParent(ImGuiLayer* parent) { m_Parent = parent; }

		virtual void OnImGUIRender() = 0;
	protected:
		PannelData* m_PannelData = nullptr;

		ImGuiLayer* m_Parent = nullptr;
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


		 void SetRenderTarget(const std::string& fbName);
		 const std::string& GetRenderTarget() { return m_DisplayedFB; }
		 void SetGuizmo(int g);

		 Camera* GetCurrentCamera();

		 CameraController* GetCameraController();

		 void ResizeCamera();
		 void ResetViewportSize();

		 void Begin();
		 void End();

		void SaveSceneImpl();
	private:
		ImGuiLayer(ImGuiLayer const&) = delete;
		ImGuiLayer& operator=(ImGuiLayer const&) = delete;

		std::vector<std::unique_ptr<Pannel>> m_Pannels;

		PannelData m_PannelData;

		CameraController* m_CameraController;

		std::string m_DisplayedFB = "FinalOutput";

		std::unique_ptr<OrthographicCameraController> m_CameraController2D;
		std::unique_ptr<PerspectiveCameraController> m_CameraController3D;

		glm::vec2 m_ViewportSize = {0,0};
		glm::vec2 m_viewportBounds[2];

		int m_GuizmoType = -1;

		bool m_FlyMode;

		bool m_CODA =  false;

		void ImGuizmoImpl();

		void InitCameraControllers();

		void WindowedEditSceneLayer();
		void WindowedSceneLayer();
		void MaximizedSceneLayer();

		void KeyboardShortcuts(const KeyPressEvent& e);

		//std::unique_ptr<SceneHierarchyPannel> m_SceneHierachyPannel;

		// Member functions for handling events
		bool OnKeyPressed(const KeyPressEvent& e);
		bool OnSceneSwitched(const SceneSwitchEvent& e);

		void NewSceneImpl();
		void SaveSceneAsImpl();
		void OpenSceneImpl();
		void SetAsDefaultSceneImpl();

		void KeyboardShortcuts();
	};

}