
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
#include "SpriteSourcePannel.h"
#include "Null/Engine/Submodules/Scene.h"
//#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>
#include "../Editors/SceneEditor.h"
#include "../../../NullEngine/src/Null/Tools/ImGuiH.h"
#include "NIncludes.h"

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
	//void SpriteSourcePannel::SetContext(Scene* scene)
	//{
	//	m_Context = scene;
	//}

	void SpriteSourcePannel::OnImGUIRender()
	{
		NSpriteSourceManager* srcManager = NSpriteSourceManager::Instance();
		auto* src = srcManager->Get(m_PannelData->selectedSpriteSourceID);
		ImGui::Begin("Sprite Source Hierarchy");

		ImGuiH::DrawDragDrop("Sprite Source Selector", m_PannelData->selectedSpriteSourceID, src, NTextureManager::Instance(), NSpriteSourceManager::Instance(), 1.250f);

		ImGui::End();
	}
}
