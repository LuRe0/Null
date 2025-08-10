
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
#include "{{PANNEL_NAME}}Pannel.h"
#include "Null/Engine/Submodules/Scene.h"
//#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>
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
	void {{PANNEL_NAME}}Pannel::OnImGUIRender()
	{
		const auto& modules = NEngine::Instance().GetModules();

		ImGui::Begin("{{PANNEL_NAME}}");

	
		ImGui::End();
	}
}
