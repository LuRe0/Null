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
	class Scene;
	class ImGuiEditor;
	using EntityID = std::uint32_t;



	class NLE_API ImGuiLayer : public ILayer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() override;

		 void OnAttach() override;

		 void OnUpdate(float dt) override;

		 void OnRender() override;

		 void OnImGUIRender();

		 void CreateMainDockspace();

		 void OnDetach() override;

		 void OnEvent(const Event& event) override;

		 void SetPannelParent();

		 void Begin();
		 void End();

		void SaveSceneImpl();


	private:
		ImGuiLayer(ImGuiLayer const&) = delete;
		ImGuiLayer& operator=(ImGuiLayer const&) = delete;


		void AddEditor(std::unique_ptr<ImGuiEditor>&& pannel);

		ImGuiEditor* m_CurrentEditor = {};

		//menu bar
		bool m_CODA = false;

		void RenderContextSensitiveMenuBar();


		std::vector<std::unique_ptr<ImGuiEditor>> m_Editors;

	};

}