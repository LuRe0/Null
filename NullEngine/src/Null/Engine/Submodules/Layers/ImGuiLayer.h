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
	class NLE_API ImGuiLayer : public ILayer
	{
	public:
		ImGuiLayer() {};
		~ImGuiLayer() override {}

		 void OnAttach() override;

		 void OnUpdate(float dt) override;

		 void OnDetach() override;


		 void OnEvent(const Event& event) override;

	private:
		ImGuiLayer(ImGuiLayer const&) = delete;
		ImGuiLayer& operator=(ImGuiLayer const&) = delete;


		// Member functions for handling events
		void OnWindowResize(const WindowResizeEvent& event);
		void OnWindowClose(const WindowCloseEvent& event);
		void OnKeyPressed(const KeyPressEvent& event);
		void OnKeyReleased(const KeyReleaseEvent& event);
		void OnKeyTyped(const KeyTypedEvent& event);
		//void OnKeyHold(const KeyHoldEvent& event);
		void OnMousePressed(const MouseButtonPressEvent& event);
		void OnMouseReleased(const MouseButtonReleaseEvent& event);
		void OnMouseMove(const MouseMoveEvent& event);
		void OnMouseScroll(const MouseScrolledEvent& event);
	};

}