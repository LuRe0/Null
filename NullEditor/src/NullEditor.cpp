#include "Null.h"
#include <iostream>
//#include "Layers/ImGuiLayer.h"	
#include "Null/Engine/Submodules/Layers/ImGuiLayer.h"
#include "Pannels/SceneHierarchyPannel.h"
#include "Pannels/ComponentInspectorPannel.h"
#include "Pannels/ScenePropertyPannel.h"
#include "Pannels/ContentBrowserPannel.h"
#include "Pannels/ModuleStatsPannel.h"
#include "Pannels/EditorConsolePannel.h"
#include "Pannels/EditorToolbarPannel.h"

namespace NULLENGINE
{
	class NullEditor : public Application
	{
	public:
		NullEditor() : Application("Null Editor")
		{
			//add create function to any modules added by user and add them to the modules json file in the order that they need to be read in
			m_NullEngine->SetEditorEnabled(true);
		}
		~NullEditor()
		{ 

		}
		void Load() override
		{
			Application::Load();

			auto gui = std::make_unique<ImGuiLayer>();

			auto pannel = std::make_unique<SceneHierarchyPannel>();
			auto pannel2 = std::make_unique<ComponentInspectorPannel>();
			auto pannel3 = std::make_unique<ScenePropertyPannel>();
			auto pannel4 = std::make_unique<ContentBrowserPannel>();
			auto pannel5 = std::make_unique<ModuleStatsPannel>();
			auto pannel6 = std::make_unique<EditorConsolePannel>();
			auto pannel7 = std::make_unique<EditorToolbarPannel>();

			gui.get()->AddPannel(std::move(pannel));
			gui.get()->AddPannel(std::move(pannel2));
			gui.get()->AddPannel(std::move(pannel3));
			gui.get()->AddPannel(std::move(pannel4));
			gui.get()->AddPannel(std::move(pannel5));
			gui.get()->AddPannel(std::move(pannel6));
			gui.get()->AddPannel(std::move(pannel7));
			
			PushLayer(std::move(gui));
		}


	private:

	};
}

NULLENGINE::Application* NULLENGINE::CreateApplication()
{
	return new NullEditor();
}