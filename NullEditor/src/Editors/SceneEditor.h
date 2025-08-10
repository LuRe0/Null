#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NStub.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "ImGuiEditor.h"
#include "Null/Engine/Submodules/Layers/Layer.h"
#include <memory>
#include <glm/glm.hpp>

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
	class Scene;
	class Camera;
	class OrthographicCameraController;
	class PerspectiveCameraController;
	class CameraController;
	class SceneEditor;
	class ScenePannelData;
	//class ScenePannel;
	class KeyPressEvent;
	class Event;
	class SceneSwitchEvent;





	class NLE_API SceneEditor : public ImGuiEditor
	{
	public:
		SceneEditor();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float dt) override;
		void OnRender() override;
		void RenderMenuBar() override; // Renders the menu bar
		bool HandleKeyboardShortcuts(const KeyPressEvent& e) override;
		void OnEvent(const Event& e) override;
		void AddPannel(std::unique_ptr<ScenePannel>&& pannel);
		void SaveChanges() override;

		void Reset() override
		{
			m_PannelData = ScenePannelData();
		}
		// Scene-specific methods
		//void AddPannel(std::unique_ptr<Pannel>&& pannel);
		void SetCamera(int type);
		void SetGuizmo(int g);
		CameraController* GetCameraController() { return m_CameraController; }
		Camera* GetCurrentCamera();

		void SetPannelData(const ScenePannelData& data);
		void SetPannelParent();


		void SetRenderTarget(const std::string& fbName);
		const std::string& GetRenderTarget() { return m_DisplayedFB; }

		static glm::vec2 GetMousePosition();

		void ResizeCamera();
		void ResetViewportSize();
		void SaveSceneImpl();


	private:
		// Your existing scene-specific members
		ScenePannelData m_PannelData;
		std::unique_ptr<OrthographicCameraController> m_CameraController2D;
		std::unique_ptr<PerspectiveCameraController> m_CameraController3D;
		std::vector<std::unique_ptr<ScenePannel>> m_Pannels;

		CameraController* m_CameraController = nullptr;
		glm::vec2 m_ViewportSize = { 0, 0 };
		glm::vec2 m_viewportBounds[2];
		int m_GuizmoType = -1;
		bool m_FlyMode = false;
		std::string m_DisplayedFB = "FinalOutput";

		// Scene-specific rendering methods
		void WindowedEditSceneLayer();
		void WindowedSceneLayer();
		void MaximizedSceneLayer();
		void ImGuizmoImpl();
		void InitCameraControllers();


		// Scene file operations
		void NewSceneImpl();
		void SaveSceneAsImpl();
		void OpenSceneImpl();
		void SetAsDefaultSceneImpl();
		void KeyboardShortcuts();

		bool OnKeyPressed(const KeyPressEvent& e);
		bool OnSceneSwitched(const SceneSwitchEvent& e);
	};


}