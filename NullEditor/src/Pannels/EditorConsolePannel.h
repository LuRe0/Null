#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	EditorConsolePannel.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Submodules/Layers/ImGuiLayer.h"
#include "spdlog/sinks/base_sink.h"
#include "spdlog/spdlog.h"

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




	class EditorConsolePannel : public Pannel
	{
	public:
		EditorConsolePannel() = default;
		~EditorConsolePannel() =  default;

		void AddLine(const std::string& str);
		void OnImGUIRender();
	private:
		EditorConsolePannel(EditorConsolePannel const&) = delete;
		EditorConsolePannel& operator=(EditorConsolePannel const&) = delete;


		//std::vector<std::string> m_ConsoleLog;

		//friend class OutputBuffer;
	};





}