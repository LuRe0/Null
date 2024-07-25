#include "Null.h"
#include <iostream>
//#include "Layers/ImGuiLayer.h"	
#include "Null/Engine/Submodules/Layers/ImGuiLayer.h"
#include "Pannels/SceneHierarchyPannel.h"

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

			NSceneManager* scMan = NEngine::Instance().Get<NSceneManager>();

			auto gui = std::make_unique<ImGuiLayer>();

			auto pannel = std::make_unique<SceneHierarchyPannel>();
			pannel.get()->SetContext(scMan->GetCurrentScene());

			gui.get()->AddPannel(std::move(pannel));
			PushLayer(std::move(gui));
		}


	private:

	};
}

NULLENGINE::Application* NULLENGINE::CreateApplication()
{
	return new NullEditor();
}