#include "Null.h"
#include <iostream>
//#include "Layers/ImGuiLayer.h"	
#include "Null/Engine/Submodules/Layers/ImGuiLayer.h"
#include "Pannels/SceneHierarchyPannel.h"
#include "Pannels/ComponentInspectorPannel.h"
#include "Pannels/ScenePropertyPannel.h"

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

			gui.get()->AddPannel(std::move(pannel));
			gui.get()->AddPannel(std::move(pannel2));
			gui.get()->AddPannel(std::move(pannel3));
			
			PushLayer(std::move(gui));
		}


	private:

	};
}

NULLENGINE::Application* NULLENGINE::CreateApplication()
{
	return new NullEditor();
}