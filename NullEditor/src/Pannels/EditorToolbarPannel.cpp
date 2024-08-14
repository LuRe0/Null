
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
#include "EditorToolbarPannel.h"
#include "Null/Engine/Submodules/Scene.h"
#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>
#include "Null/Tools/ImguiSink.h"
//#include "backends/imgui_impl_opengl3.h"
//#include "backends/imgui_impl_glfw.h"

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	//void EditorToolbarPannel::SetContext(Scene* scene)
	//{
	//	m_Context = scene;
	//}


	EditorToolbarPannel::EditorToolbarPannel()
	{
		m_PlayButton = Texture(std::string("../Assets/Resources/Textures/PlayButton.png"), true);
		m_StopButton = Texture(std::string("../Assets/Resources/Textures/StopButton.png"), true);

		m_PlayButton.Init();
		m_StopButton.Init();
	}

	void EditorToolbarPannel::OnImGUIRender()
	{
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();
		NDebugManager* debugManager = NEngine::Instance().Get<NDebugManager>();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::Begin("##UI_Toolbar", nullptr, windowFlags);

		float size = ImGui::GetWindowHeight() - 32.0f;

		ImGui::SameLine((ImGui::GetContentRegionMax().x * 0.75f));

		// Start the combo
		if (ImGui::BeginCombo("Window Mode", m_CurrentMode == MODE_MAXIMIZED ? "Maximized" : "Windowed"))
		{
			// Option to set to Maximized
			if (ImGui::Selectable("Maximized", m_CurrentMode == MODE_MAXIMIZED))
			{
				m_CurrentMode = MODE_MAXIMIZED;
			}

			// Option to set to Windowed
			if (ImGui::Selectable("Windowed", m_CurrentMode == MODE_WINDOWED))
			{
				m_CurrentMode = MODE_WINDOWED;
			}

			ImGui::EndCombo();
		}

		ImGui::SameLine((ImGui::GetContentRegionMax().x * 0.50f) - (size * 0.5f));

		ImGui::Text("Show Debug: "); ImGui::SameLine(); ImGui::Checkbox("##showdebug", &debugManager->m_ShowDebug);

		ImGui::SameLine((ImGui::GetContentRegionMax().x * 0.0f) + (size * 0.5f));

		Texture& icon = NEngine::Instance().GetEngineState() == IEngine::EDIT ? m_PlayButton : m_StopButton;

		if (ImGui::ImageButton((ImTextureID)icon.GetID(), ImVec2(size,size)))
		{
			switch (NEngine::Instance().GetEngineState())
			{
			case NULLENGINE::IEngine::EDIT:
			{
				if (m_CurrentMode == MODE_WINDOWED)
				{
					NEngine::Instance().SetEngineState(IEngine::RUN_WINDOWED);
					eventManager->QueueEvent(std::make_unique<EngineRunStateEvent>(NEngine::RUN_WINDOWED));
					m_Parent->ResetViewportSize();
				}
				else
				{
					NEngine::Instance().SetEngineState(IEngine::RUN_MAXIMIZED);
					eventManager->QueueEvent(std::make_unique<EngineRunStateEvent>(NEngine::RUN_MAXIMIZED));
				}


				m_Parent->SaveSceneImpl();

				break;
			}
			case NULLENGINE::IEngine::PAUSE:
				break;
			case NULLENGINE::IEngine::RUN_MAXIMIZED: // Handle maximized state
			case NULLENGINE::IEngine::RUN_WINDOWED:  // Handle windowed state
				NEngine::Instance().SetEngineState(IEngine::EDIT);
				eventManager->QueueEvent(std::make_unique<SceneSwitchEvent>(m_PannelData->m_Context->GetName(), 
																			m_PannelData->m_Context->GetName()));
				eventManager->QueueEvent(std::make_unique<EngineEditStateEvent>(NEngine::EDIT));
				break;
			case NULLENGINE::IEngine::SIMULATE:
				break;
			default:
				break;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar(2);
	}
}
