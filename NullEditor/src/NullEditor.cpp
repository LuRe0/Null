#include "Null.h"
#include <iostream>
//#include "Layers/ImGuiLayer.h"	
#include "Null/Engine/Submodules/Layers/ImGuiLayer.h"
#include "Pannels/SceneHierarchyPannel.h"
#include "Pannels/ComponentInspectorPannel.h"

namespace NULLENGINE
{
	class NullEditor : public Application
	{
	public:
		NullEditor() : Application("Null Editor")
		{
			//add create function to any modules added by user and add them to the modules json file in the order that they need to be read in
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

			gui.get()->AddPannel(std::move(pannel));
			gui.get()->AddPannel(std::move(pannel2));
			
			PushLayer(std::move(gui));
		}


	private:

	};
}

NULLENGINE::Application* NULLENGINE::CreateApplication()
{
	return new NullEditor();
}