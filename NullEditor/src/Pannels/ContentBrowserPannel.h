#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	ContentBrowserPannel.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Submodules/Layers/ImGuiLayer.h"

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

	class ContentBrowserPannel : public Pannel
	{
	public:
		ContentBrowserPannel();
		~ContentBrowserPannel() = default;

		void OnImGUIRender();
	private:
		ContentBrowserPannel(ContentBrowserPannel const&) = delete;
		ContentBrowserPannel& operator=(ContentBrowserPannel const&) = delete;

		std::filesystem::path m_CurrentDirectory;

	};

}