#include "Null.h"
#include <iostream>
#include "Null/Engine/Submodules/Layers/ImGuiLayer.h"

class Sandbox : public NULLENGINE::Application
{
public:
	Sandbox() : Application()
	{
	}
	~Sandbox()
	{

	}
	void Load() override
	{
		Application::Load();
		PushOverlay(std::move(std::make_unique<NULLENGINE::ImGuiLayer>()));
	}

private:

};

NULLENGINE::Application* NULLENGINE::CreateApplication()
{
	return new Sandbox;
}