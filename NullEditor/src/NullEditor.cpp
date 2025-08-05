#include "Null.h"
#include <iostream>
//#include "Layers/ImGuiLayer.h"	
#include "Null/Engine/Submodules/Layers/ImGuiLayer.h"

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
			
			PushLayer(std::move(gui));
		}


	private:

	};
}

NULLENGINE::Application* NULLENGINE::CreateApplication()
{
	return new NullEditor();
}